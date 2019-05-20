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
    this->ui->label_7->setText("\346\255\243\345\234\250\346\243\200\346\265\213\345\275\223\344\270\255..."); //正在检测当中...
    this->ui->stackedWidget->setCurrentIndex(1);
    //this->ui->label_7->setText("\346\243\200\346\265\213\345\244\261\350\264\245"); //检测失败
}


void ScreenCheck::addlabel(const QString &name, const QString &content)
{
    this->object_label_map[name] = new Label(this);
    this->object_label_map[name]->setText(content);
    this->ui->verticalLayout_4->addWidget(this->object_label_map[name]);
}


void ScreenCheck::on_pushButton_4_released()
{
    emit tell_window_step_page(1);
}

void ScreenCheck::on_pushButton_8_released()
{
    this->ui->stackedWidget->setCurrentIndex(0);
}
