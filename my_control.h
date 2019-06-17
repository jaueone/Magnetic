#ifndef MY_CONTROL_H
#define MY_CONTROL_H
#include <QLabel>
#include <QComboBox>
#include <QDebug>
#include <QPixmap>
#include <QPainterPath>
#include <QPainter>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QThread>
#include <QTime>
#include <QCoreApplication>

static QSqlDatabase __database__;


class Label : public QLabel
{
    Q_OBJECT
public:
    Label(QWidget *parent=nullptr):QLabel(parent) {
        this->setStyleSheet("QLabel{color:white;font-family:Microsoft YaHei; font-size:25px; background-color:rgb(132,151,176);}");
        this->setMinimumHeight(30);
        this->setMaximumHeight(30);
    }
    virtual ~Label() {}
    static QPixmap draw_ellipse(const QString &url){
        QPixmap pixmapa(url);
        QPixmap pixmap(130,130);
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        QPainterPath path;
        path.addEllipse(0, 0, 130, 130);
        painter.setClipPath(path);
        painter.drawPixmap(0, 0, 130, 130, pixmapa);
        return pixmap;
    }
};

class ComboBox: public QComboBox
{
    Q_OBJECT
public:
    ComboBox(QWidget *parent = nullptr):QComboBox(parent) {

}
    void mousePressEvent(QMouseEvent *e){
        QComboBox::mousePressEvent(e);
        emit clicked();
    }
signals:
    void clicked();
};

class DB
{
public:
    static QSqlDatabase * getInterface(){
        if (QSqlDatabase::contains("qt_sql_default_connection"))
        {
            __database__ = QSqlDatabase::database("qt_sql_default_connection");
            return &__database__;
        }
        else {
            __database__ = QSqlDatabase::addDatabase("QSQLITE");
            __database__.setDatabaseName("./database/MyDataBase.db");
            __database__.setUserName("XingYeZhiXia");
            __database__.setPassword("123456");
            return &__database__;
        }
    }

protected:
    DB() {}
    DB(const DB& other){}
    ~DB(){}
};
class Sleep
{
public:
    Sleep() {}
    static void sleep(unsigned int msec)
    {
        QTime dieTime = QTime::currentTime().addMSecs(msec);
        while( QTime::currentTime() < dieTime )
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
};
#endif
