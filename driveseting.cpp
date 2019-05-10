#include "driveseting.h"
#include "ui_driveseting.h"

DriveSeting::DriveSeting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DriveSeting)
{
    ui->setupUi(this);
}

DriveSeting::~DriveSeting()
{
    delete ui;
}
