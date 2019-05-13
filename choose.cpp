#include "choose.h"
#include "ui_choose.h"
#include <QPixmap>


Choose::Choose(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Choose)
{
    ui->setupUi(this);
}

Choose::~Choose()
{
    delete ui;
}

void Choose::setMen(const Meninfo &info)
{
    this->ui->label_2->setText(info.name);
    this->ui->label->setStyleSheet(QString("min-width:  110px;\
                                           max-width: 110px;\
                                           min-height: 110px;\
                                           max-height: 110px;  \
                                           border-radius: 55px;\
                                           border:1px solid blue;   \
                                           border-image: url(%1);").arg(info.picture));
    if (info.isengineer == "yes")
    {
        this->ui->pushButton_3->hide();
    }
    else if (info.isengineer == "no") {
        this->ui->pushButton_3->show();
    }
}

void Choose::on_pushButton_2_released()
{
    emit tell_window_step_page(3);
}

void Choose::on_pushButton_released()
{
    emit tell_window_step_page(4);
}

void Choose::on_pushButton_4_released()
{
    emit tell_window_step_page(0);
}

void Choose::on_pushButton_3_released()
{
    emit tell_window_step_page(2);
}
