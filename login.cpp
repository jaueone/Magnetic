#include "login.h"
#include "ui_login.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlRecord>

Login::Login(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    this->ui->label->hide();
    this->ui->lineEdit->setPlaceholderText("工号");
    this->ui->lineEdit_2->setPlaceholderText("姓名");
}

Login::~Login()
{
    delete ui;
}


void Login::on_pushButton_released()
{
    emit tell_window_step_page2(1);
}

void Login::on_lineEdit_textChanged(const QString &arg1)
{
    Meninfo info = this->search_meninfo(arg1);
    if (info.name == "" && info.number ==""){
        return;
    }
    else {
        this->ui->lineEdit_2->setText(info.name);
        this->ui->lineEdit->setText(info.number);
        this->ui->label->setStyleSheet(QString("min-width:  110px;\
                                               max-width: 110px;\
                                               min-height: 110px;\
                                               max-height: 110px;  \
                                               border-radius: 55px;\
                                               border:1px solid blue;   \
                                               border-image: url(%1);").arg(info.pixture));
    }
}

Meninfo Login::search_meninfo(const QString &number)
{
    if (QSqlDatabase::contains("qt_sql_default_connection"))
    {
        database = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        database = QSqlDatabase::addDatabase("QSQLITE");
        database.setDatabaseName("MyDataBase.db");
        database.setUserName("XingYeZhiXia");
        database.setPassword("123456");
    }
    if (!database.open())
    {
        qDebug() << "Error: Failed to connect database." << database.lastError();
    }
    else
    {
        Meninfo info;
        QSqlQuery query(database);
        QString str = QString("SELECT name,picture FROM men WHERE number = '%1'").arg(number);
        qDebug() << str;
        if (!query.exec(str)){
            qDebug() << "error";
        }
        else {
            QSqlRecord record = query.record();
            if (record.isEmpty())
                qDebug() << "empty";
            qDebug() << "get"  << record.value("name");
            return info;
        }
    }
}
