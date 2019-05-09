#ifndef MY_CONTROL_H
#define MY_CONTROL_H
#include <QLabel>

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
};
#endif // MY_CONTROL_H
