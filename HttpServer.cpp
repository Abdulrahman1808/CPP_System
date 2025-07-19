#include "HttpServer.h"
#include <QDebug>
#include <QDateTime>
#include <QUrlQuery>

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
    server->route("/api/sales", QHttpServerRequest::Method::GET, 
                  [this](const QHttpServerRequest &request, QHttpServerResponder &&responder) {
                      handleGetSales(request, std::move(responder));
                  });

    server->route("/api/inventory", QHttpServerRequest::Method::GET,
                  [this](const QHttpServerRequest &request, QHttpServerResponder &&responder) {
                      handleGetInventory(request, std::move(responder));
                  });

    server->route("/api/activity-log", QHttpServerRequest::Method::GET,
                  [this](const QHttpServerRequest &request, QHttpServerResponder &&responder) {
                      handleGetActivityLog(request, std::move(responder));
                  });

    server->route("/api/summary", QHttpServerRequest::Method::GET,
                  [this](const QHttpServerRequest &request, QHttpServerResponder &&responder) {
                      handleGetSummary(request, std::move(responder));
                  });

    server->route("/api/sales", QHttpServerRequest::Method::POST,
                  [this](const QHttpServerRequest &request, QHttpServerResponder &&responder) {
                      handlePostSale(request, std::move(responder));
                  });

    // Add CORS headers
    server->afterRequest([](QHttpServerResponse &&resp) {
        resp.setHeader("Access-Control-Allow-Origin", "*");
        resp.setHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        resp.setHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
        return std::move(resp);
    });

    if (server->listen(QHostAddress::Any, port)) {
        qDebug() << "HTTP Server started on port" << port;
        qDebug() << "API Endpoints:";
        qDebug() << "  GET  /api/sales - Get sales data";
        qDebug() << "  GET  /api/inventory - Get inventory status";
        qDebug() << "  GET  /api/activity-log - Get recent activity";
        qDebug() << "  GET  /api/summary - Get summary statistics";
        qDebug() << "  POST /api/sales - Create new sale (admin only)";
        return true;
    } else {
        qDebug() << "Failed to start HTTP Server on port" << port;
        return false;
    }
}

void HttpServer::stop()
{
    if (server) {
        server->close();
        delete server;
        server = nullptr;
        qDebug() << "HTTP Server stopped";
    }
}

void HttpServer::handleGetSales(const QHttpServerRequest &request, QHttpServerResponder &&responder)
{
    if (!validateApiKey(request)) {
        responder.sendJson(createErrorResponse("Invalid API key"), 401);
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT s.id, s.cashier, s.sale_time, s.total, s.payment_method, "
                  "COUNT(si.id) as item_count FROM sales s "
                  "LEFT JOIN sales_items si ON s.id = si.sale_id "
                  "GROUP BY s.id ORDER BY s.sale_time DESC LIMIT 50");
    
    if (query.exec()) {
        QJsonArray salesArray = queryToJsonArray(query);
        responder.sendJson(createSuccessResponse(salesArray));
    } else {
        responder.sendJson(createErrorResponse("Database error: " + query.lastError().text()), 500);
    }
}

void HttpServer::handleGetInventory(const QHttpServerRequest &request, QHttpServerResponder &&responder)
{
    if (!validateApiKey(request)) {
        responder.sendJson(createErrorResponse("Invalid API key"), 401);
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT id, name, category, price, quantity, min_stock, description FROM products ORDER BY name");
    
    if (query.exec()) {
        QJsonArray inventoryArray = queryToJsonArray(query);
        responder.sendJson(createSuccessResponse(inventoryArray));
    } else {
        responder.sendJson(createErrorResponse("Database error: " + query.lastError().text()), 500);
    }
}

void HttpServer::handleGetActivityLog(const QHttpServerRequest &request, QHttpServerResponder &&responder)
{
    if (!validateApiKey(request)) {
        responder.sendJson(createErrorResponse("Invalid API key"), 401);
        return;
    }

    QSqlQuery query;
    query.prepare("SELECT username, action, details, timestamp FROM activity_log ORDER BY timestamp DESC LIMIT 100");
    
    if (query.exec()) {
        QJsonArray logArray = queryToJsonArray(query);
        responder.sendJson(createSuccessResponse(logArray));
    } else {
        responder.sendJson(createErrorResponse("Database error: " + query.lastError().text()), 500);
    }
}

void HttpServer::handleGetSummary(const QHttpServerRequest &request, QHttpServerResponder &&responder)
{
    if (!validateApiKey(request)) {
        responder.sendJson(createErrorResponse("Invalid API key"), 401);
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
    
    responder.sendJson(createSuccessResponse(summary));
}

void HttpServer::handlePostSale(const QHttpServerRequest &request, QHttpServerResponder &&responder)
{
    if (!validateApiKey(request)) {
        responder.sendJson(createErrorResponse("Invalid API key"), 401);
        return;
    }

    // Parse JSON body
    QJsonDocument doc = QJsonDocument::fromJson(request.body());
    if (!doc.isObject()) {
        responder.sendJson(createErrorResponse("Invalid JSON"), 400);
        return;
    }

    QJsonObject saleData = doc.object();
    QString cashier = saleData["cashier"].toString();
    double total = saleData["total"].toDouble();
    QString paymentMethod = saleData["payment_method"].toString();
    QJsonArray items = saleData["items"].toArray();

    if (cashier.isEmpty() || total <= 0 || paymentMethod.isEmpty()) {
        responder.sendJson(createErrorResponse("Missing required fields"), 400);
        return;
    }

    // Insert sale
    QSqlQuery saleQuery;
    saleQuery.prepare("INSERT INTO sales (cashier, sale_time, total, payment_method) VALUES (?, ?, ?, ?) RETURNING id");
    saleQuery.addBindValue(cashier);
    saleQuery.addBindValue(QDateTime::currentDateTime());
    saleQuery.addBindValue(total);
    saleQuery.addBindValue(paymentMethod);

    if (!saleQuery.exec() || !saleQuery.next()) {
        responder.sendJson(createErrorResponse("Failed to create sale"), 500);
        return;
    }

    int saleId = saleQuery.value(0).toInt();

    // Insert sale items
    for (const QJsonValue &itemValue : items) {
        QJsonObject item = itemValue.toObject();
        QString productName = item["product_name"].toString();
        int quantity = item["quantity"].toInt();
        double price = item["price"].toDouble();

        QSqlQuery itemQuery;
        itemQuery.prepare("INSERT INTO sales_items (sale_id, product_name, quantity, price) VALUES (?, ?, ?, ?)");
        itemQuery.addBindValue(saleId);
        itemQuery.addBindValue(productName);
        itemQuery.addBindValue(quantity);
        itemQuery.addBindValue(price);
        itemQuery.exec();
    }

    QJsonObject response;
    response["sale_id"] = saleId;
    response["message"] = "Sale created successfully";
    
    responder.sendJson(createSuccessResponse(response));
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
            
            if (value.type() == QVariant::DateTime) {
                obj[fieldName] = value.toDateTime().toString(Qt::ISODate);
            } else if (value.type() == QVariant::Double) {
                obj[fieldName] = value.toDouble();
            } else if (value.type() == QVariant::Int) {
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