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
#include <QHostAddress>

class HttpServer : public QObject
{
    Q_OBJECT

public:
    explicit HttpServer(QObject *parent = nullptr);
    ~HttpServer();

    bool start(int port = 8080);
    void stop();

private:
    void handleGetSales(const QHttpServerRequest &request, QHttpServerResponder &responder);
    void handleGetInventory(const QHttpServerRequest &request, QHttpServerResponder &responder);
    void handleGetActivityLog(const QHttpServerRequest &request, QHttpServerResponder &responder);
    void handleGetSummary(const QHttpServerRequest &request, QHttpServerResponder &responder);

    QHttpServer *server;
    bool validateApiKey(const QHttpServerRequest &request);
    QJsonObject createSuccessResponse(const QJsonValue &data);
    QJsonObject createErrorResponse(const QString &message);
    QJsonArray queryToJsonArray(QSqlQuery &query);
    QString apiKey = "pos_api_key_2024";
};

#endif // HTTPSERVER_H 