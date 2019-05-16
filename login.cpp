#include "login.h"
#include "ui_login.h"
#include "my_control.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QProcess>
#include <QPixmap>


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
    Meninfo info = this->search_meninfo(this->ui->lineEdit->text());
    if (info.name == this->ui->lineEdit_2->text() && this->ui->lineEdit_2->text() != ""){
        emit tell_window_step_page(1);
    }

}

void Login::on_lineEdit_textChanged(const QString &arg1)
{
    if (arg1 == ""){
        this->ui->lineEdit_2->clear();
        this->ui->label->hide();
        return;
    }

    Meninfo info = this->search_meninfo(arg1);
    if (info.name == ""){
        return;
    }
    else {

        QPixmap pixmap = Label::draw_ellipse(QString("./picture/%1").arg(info.picture));
        this->ui->label->setPixmap(pixmap);
        this->ui->lineEdit_2->setText(info.name);
        this->ui->lineEdit->setText(info.number);


        this->ui->label->show();
        emit tell_window_men_login(info);
    }
}

Meninfo Login::search_meninfo(const QString &number)
{
    Meninfo info;

    QSqlDatabase *database = DB::interface();
    if (!database->open())
    {
        qDebug() << "Error: Failed to connect database." << database->lastError();
        return info;
    }
    else
    {
        QSqlQuery query(*database);
        QString str = QString("SELECT * FROM men WHERE number = \"%1\"").arg(number);
        qDebug() << str;
        if (!query.exec(str)){
            qDebug() << "error";
        }
        else {
            while (query.next()) {
                QSqlRecord record = query.record();
                info.name = record.value("name").toString();
                info.picture = record.value("picture").toString();
                info.isengineer = record.value("isengineer").toString();
                info.number = number;
                qDebug() << info.name;
            }
           database->close();
           return info;
        }
    }
}

void Login::clear()
{
    this->ui->label->hide();
    this->ui->lineEdit->clear();
    this->ui->lineEdit_2->clear();
}





