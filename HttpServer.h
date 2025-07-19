#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include <QHttpServer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

class HttpServer : public QObject
{
    Q_OBJECT

public:
    explicit HttpServer(QObject *parent = nullptr);
    ~HttpServer();

    bool start(int port = 8080);
    void stop();

private slots:
    void handleGetSales(const QHttpServerRequest &request, QHttpServerResponder &&responder);
    void handleGetInventory(const QHttpServerRequest &request, QHttpServerResponder &&responder);
    void handleGetActivityLog(const QHttpServerRequest &request, QHttpServerResponder &&responder);
    void handleGetSummary(const QHttpServerRequest &request, QHttpServerResponder &&responder);
    void handlePostSale(const QHttpServerRequest &request, QHttpServerResponder &&responder);

private:
    QHttpServer *server;
    QJsonObject createErrorResponse(const QString &message);
    QJsonObject createSuccessResponse(const QJsonValue &data);
    QJsonArray queryToJsonArray(QSqlQuery &query);
    bool validateApiKey(const QHttpServerRequest &request);
    
    // API Key for basic authentication (in production, use proper auth)
    QString apiKey = "pos_api_key_2024";
};

#endif // HTTPSERVER_H 