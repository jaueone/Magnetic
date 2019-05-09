#include "screencheck.h"
#include "ui_screencheck.h"


ScreenCheck::ScreenCheck(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScreenCheck)
{
    ui->setupUi(this);

}

ScreenCheck::~ScreenCheck()
{
    delete ui;
}

void ScreenCheck::on_pushButton_released()
{
    this->ui->stackedWidget->setCurrentIndex(1);
}

void ScreenCheck::on_pushButton_2_released()
{
    this->ui->stackedWidget->setCurrentIndex(2);
}

void ScreenCheck::on_pushButton_3_released()
{
    this->ui->stackedWidget->setCurrentIndex(0);
}

void ScreenCheck::addlabel(const QString &name, const QString &content)
{
    this->object_label_map[name] = new Label(this);
    this->object_label_map[name]->setText(content);
    this->ui->verticalLayout_4->addWidget(this->object_label_map[name]);
}

void ScreenCheck::on_pushButton_4_clicked()
{
    nae += "s";
    this->addlabel(nae, nae);
}
