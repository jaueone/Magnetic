#include "choose.h"
#include "my_control.h"
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
    QPixmap pixmap = Label::draw_ellipse(QString("./picture/%1").arg(info.picture));
    this->ui->label->setPixmap(pixmap);

    if (info.isengineer == "yes")
    {
        this->ui->pushButton_3->show();
    }
    else if (info.isengineer == "no") {
        this->ui->pushButton_3->hide();
    }
}

void Choose::on_pushButton_2_released()
{
    emit tell_window_check_self();
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


