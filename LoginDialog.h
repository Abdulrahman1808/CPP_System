#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

class QLineEdit;
class QPushButton;

class LoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit LoginDialog(QWidget* parent = nullptr);
    QString getUsername() const;
    QString getPassword() const;
signals:
    void loginFailed(const QString& message);
private slots:
    void tryLogin();
private:
    QLineEdit* usernameEdit;
    QLineEdit* passwordEdit;
    QPushButton* loginButton;
};

#endif // LOGINDIALOG_H 