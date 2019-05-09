#ifndef LOGIN_H
#define LOGIN_H

#include <QFrame>
#include <QSqlDatabase>
#include <QSqlError>

namespace Ui {
class Login;
}

class Meninfo
{
public:
    Meninfo() {}
    QString number = "";
    QString name = "";
    QString pixture = "";
};

class Login : public QFrame
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();
    Meninfo search_meninfo(const QString &number);

private slots:
    void on_pushButton_released();
    void on_lineEdit_textChanged(const QString &arg1);

signals:
    void tell_window_step_page2(int page);

private:
    Ui::Login *ui;
    QSqlDatabase database;
};

#endif // LOGIN_H
