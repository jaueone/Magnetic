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

void Choose::on_pushButton_2_released()
{
    emit tell_window_step_page3(2);
}

void Choose::on_pushButton_released()
{
    emit tell_window_step_page3(3);
}
