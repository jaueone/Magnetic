#ifndef MY_CONTROL_H
#define MY_CONTROL_H
#include <QLabel>
#include <QComboBox>
#include <QDebug>
#include <QPixmap>
#include <QPainterPath>
#include <QPainter>

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
        emit tell_serial_scan();
    }
signals:
    void tell_serial_scan();
};

#endif // MY_CONTROL_H


