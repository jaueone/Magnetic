#include "screenresult.h"
#include "ui_screenresult.h"

ScreenResult::ScreenResult(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScreenResult)
{
    ui->setupUi(this);
}

ScreenResult::~ScreenResult()
{
    delete ui;
}
