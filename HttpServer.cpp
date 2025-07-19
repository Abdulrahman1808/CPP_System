#include "HttpServer.h"
#include <QDebug>
#include <QDateTime>
#include <QUrlQuery>
#include <QSqlRecord>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSqlQuery>
#include <QVariant>
#include <QHostAddress>
#include <QSqlError>

HttpServer::HttpServer(QObject *parent) : QObject(parent), server(nullptr)
{
}

HttpServer::~HttpServer()
{
    stop();
}

bool HttpServer::start(int port)
{
    if (server) {
        qDebug() << "HTTP Server already running";
        return false;
    }

    server = new QHttpServer(this);

    // Set up routes
    server->route("/api/sales", QHttpServerRequest::Method::Get, 
                  [this](const QHttpServerRequest &request, QHttpServerResponder &responder) {
                      handleGetSales(request, responder);
                  });

    server->route("/api/inventory", QHttpServerRequest::Method::Get,
                  [this](const QHttpServerRequest &request, QHttpServerResponder &responder) {
                      handleGetInventory(request, responder);
                  });

    server->route("/api/activity-log", QHttpServerRequest::Method::Get,
                  [this](const QHttpServerRequest &request, QHttpServerResponder &responder) {
                      handleGetActivityLog(request, responder);
                  });

    server->route("/api/summary", QHttpServerRequest::Method::Get,
                  [this](const QHttpServerRequest &request, QHttpServerResponder &responder) {
                      handleGetSummary(request, responder);
                  });

    // Try to start the server
    try {
        qDebug() << "HTTP Server started on port" << port;
        qDebug() << "API Endpoints:";
        qDebug() << "  GET  /api/sales - Get sales data";
        qDebug() << "  GET  /api/inventory - Get inventory status";
        qDebug() << "  GET  /api/activity-log - Get recent activity";
        qDebug() << "  GET  /api/summary - Get summary statistics";
        return true;
    } catch (...) {
        qDebug() << "Failed to start HTTP Server on port" << port;
        return false;
    }
}

void HttpServer::stop()
{
    if (server) {
        delete server;
        server = nullptr;
        qDebug() << "HTTP Server stopped";
    }
}

void HttpServer::handleGetSales(const QHttpServerRequest &request, QHttpServerResponder &responder)
{
    if (!validateApiKey(request)) {
        responder.write(QJsonDocument(createErrorResponse("Invalid API key")).toJson(), "application/json");
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT s.id, s.cashier, s.sale_time, s.total, s.payment_method, "
                  "COUNT(si.id) as item_count FROM sales s "
                  "LEFT JOIN sales_items si ON s.id = si.sale_id "
                  "GROUP BY s.id ORDER BY s.sale_time DESC LIMIT 50");
    
    if (query.exec()) {
        QJsonArray salesArray = queryToJsonArray(query);
        responder.write(QJsonDocument(createSuccessResponse(salesArray)).toJson(), "application/json");
    } else {
        responder.write(QJsonDocument(createErrorResponse("Database error: " + query.lastError().text())).toJson(), "application/json");
    }
}

void HttpServer::handleGetInventory(const QHttpServerRequest &request, QHttpServerResponder &responder)
{
    if (!validateApiKey(request)) {
        responder.write(QJsonDocument(createErrorResponse("Invalid API key")).toJson(), "application/json");
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT id, name, category, price, quantity, min_stock, description FROM products ORDER BY name");
    
    if (query.exec()) {
        QJsonArray inventoryArray = queryToJsonArray(query);
        responder.write(QJsonDocument(createSuccessResponse(inventoryArray)).toJson(), "application/json");
    } else {
        responder.write(QJsonDocument(createErrorResponse("Database error: " + query.lastError().text())).toJson(), "application/json");
    }
}

void HttpServer::handleGetActivityLog(const QHttpServerRequest &request, QHttpServerResponder &responder)
{
    if (!validateApiKey(request)) {
        responder.write(QJsonDocument(createErrorResponse("Invalid API key")).toJson(), "application/json");
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT username, action, details, timestamp FROM activity_log ORDER BY timestamp DESC LIMIT 100");
    
    if (query.exec()) {
        QJsonArray logArray = queryToJsonArray(query);
        responder.write(QJsonDocument(createSuccessResponse(logArray)).toJson(), "application/json");
    } else {
        responder.write(QJsonDocument(createErrorResponse("Database error: " + query.lastError().text())).toJson(), "application/json");
    }
}

void HttpServer::handleGetSummary(const QHttpServerRequest &request, QHttpServerResponder &responder)
{
    if (!validateApiKey(request)) {
        responder.write(QJsonDocument(createErrorResponse("Invalid API key")).toJson(), "application/json");
        return;
    }

    QJsonObject summary;
    
    // Total sales
    QSqlQuery salesQuery;
    salesQuery.prepare("SELECT COUNT(*) as count, COALESCE(SUM(total), 0) as total FROM sales");
    if (salesQuery.exec() && salesQuery.next()) {
        summary["total_sales"] = salesQuery.value("total").toDouble();
        summary["total_orders"] = salesQuery.value("count").toInt();
    }
    
    // Today's sales
    QSqlQuery todayQuery;
    todayQuery.prepare("SELECT COUNT(*) as count, COALESCE(SUM(total), 0) as total FROM sales WHERE DATE(sale_time) = CURRENT_DATE");
    if (todayQuery.exec() && todayQuery.next()) {
        summary["today_sales"] = todayQuery.value("total").toDouble();
        summary["today_orders"] = todayQuery.value("count").toInt();
    }
    
    // Low stock items
    QSqlQuery lowStockQuery;
    lowStockQuery.prepare("SELECT COUNT(*) as count FROM products WHERE quantity <= min_stock");
    if (lowStockQuery.exec() && lowStockQuery.next()) {
        summary["low_stock_items"] = lowStockQuery.value("count").toInt();
    }
    
    // Total products
    QSqlQuery productsQuery;
    productsQuery.prepare("SELECT COUNT(*) as count FROM products");
    if (productsQuery.exec() && productsQuery.next()) {
        summary["total_products"] = productsQuery.value("count").toInt();
    }
    
    responder.write(QJsonDocument(createSuccessResponse(summary)).toJson(), "application/json");
}

QJsonObject HttpServer::createErrorResponse(const QString &message)
{
    QJsonObject response;
    response["success"] = false;
    response["error"] = message;
    return response;
}

QJsonObject HttpServer::createSuccessResponse(const QJsonValue &data)
{
    QJsonObject response;
    response["success"] = true;
    response["data"] = data;
    return response;
}

QJsonArray HttpServer::queryToJsonArray(QSqlQuery &query)
{
    QJsonArray array;
    while (query.next()) {
        QJsonObject obj;
        for (int i = 0; i < query.record().count(); ++i) {
            QString fieldName = query.record().fieldName(i);
            QVariant value = query.value(i);
            
            if (value.metaType() == QMetaType::fromType<QDateTime>()) {
                obj[fieldName] = value.toDateTime().toString(Qt::ISODate);
            } else if (value.metaType() == QMetaType::fromType<double>()) {
                obj[fieldName] = value.toDouble();
            } else if (value.metaType() == QMetaType::fromType<int>()) {
                obj[fieldName] = value.toInt();
            } else {
                obj[fieldName] = value.toString();
            }
        }
        array.append(obj);
    }
    return array;
}

bool HttpServer::validateApiKey(const QHttpServerRequest &request)
{
    QString authHeader = request.value("Authorization");
    if (authHeader.startsWith("Bearer ")) {
        QString key = authHeader.mid(7); // Remove "Bearer "
        return key == apiKey;
    }
    return false;
} 