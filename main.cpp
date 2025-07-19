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

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // --- PostgreSQL Connection Setup ---
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("serveo.net");          // Serveo host
    db.setPort(55432);                     // Your forwarded port (from ssh -R command)
    db.setDatabaseName("MonsterDB");       // Keep your DB name
    db.setUserName("postgres");            // Keep your username
    db.setPassword("Monsterxd2005@#@#");   // Keep your password


    if (!db.open()) {
        qDebug() << "Database error:" << db.lastError().text();
        return -1; // Exit if DB connection fails
    } else {
        qDebug() << "Connected to PostgreSQL!";
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