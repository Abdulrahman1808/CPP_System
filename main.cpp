#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include "MainWindow.h"
#include "LoginDialog.h"
#include <QSqlQuery>
#include <QVariant>
#include <QCryptographicHash>
#include <QMessageBox>
#include "ReportsScreen.h"
#include "HttpServer.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // --- PostgreSQL Connection Setup ---
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("ep-proud-morning-a2qt65e9-pooler.eu-central-1.aws.neon.tech");
    db.setPort(5432); // Default PostgreSQL port
    db.setDatabaseName("neondb");
    db.setUserName("neondb_owner");
    db.setPassword("npg_nvHxBN9Pb5ID");
    db.setConnectOptions("sslmode=require"); // Enable SSL for Neon

    if (!db.open()) {
        qDebug() << "Database error:" << db.lastError().text();
        return -1; // Exit if DB connection fails
    } else {
        qDebug() << "Connected to PostgreSQL!";
    }

    // --- Start HTTP Server ---
    HttpServer httpServer;
    if (httpServer.start(8080)) {
        qDebug() << "HTTP API Server started on port 8080";
        qDebug() << "Your coworkers can now access:";
        qDebug() << "  http://YOUR_IP:8080/api/sales";
        qDebug() << "  http://YOUR_IP:8080/api/inventory";
        qDebug() << "  http://YOUR_IP:8080/api/activity-log";
        qDebug() << "  http://YOUR_IP:8080/api/summary";
        qDebug() << "API Key: pos_api_key_2024";
    } else {
        qDebug() << "Warning: Failed to start HTTP server";
    }

    // --- User Login ---
    QString loggedInUser, userRole;
    while (true) {
        LoginDialog loginDlg;
        if (loginDlg.exec() != QDialog::Accepted) {
            return 0; // User cancelled login
        }
        QString username = loginDlg.getUsername();
        QString password = loginDlg.getPassword();
        QSqlQuery query;
        query.prepare("SELECT password_hash, role FROM users WHERE username = ?");
        query.addBindValue(username);
        if (!query.exec() || !query.next()) {
            QMessageBox::warning(nullptr, "Login Failed", "Invalid username or password.");
            continue;
        }
        QString dbHash = query.value(0).toString();
        QString role = query.value(1).toString();
        // Use pgcrypto's crypt() to check password
        QSqlQuery checkQuery;
        checkQuery.prepare("SELECT (password_hash = crypt(?, password_hash)) FROM users WHERE username = ?");
        checkQuery.addBindValue(password);
        checkQuery.addBindValue(username);
        if (!checkQuery.exec() || !checkQuery.next() || !checkQuery.value(0).toBool()) {
            QMessageBox::warning(nullptr, "Login Failed", "Invalid username or password.");
            continue;
        }
        loggedInUser = username;
        userRole = role;
        // Log login event
        ReportsScreen::logActivity(loggedInUser, "Login", "User logged in");
        break;
    }

    MainWindow w;
    w.setUserRole(userRole);
    w.setUsername(loggedInUser); // Pass username to MainWindow/SalesScreen
    w.show();
    return app.exec();
}