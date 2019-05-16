#include "main_page.h"
#include "ui_main_page.h"

#include <QDebug>

MainPage::MainPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainPage)
{
    ui->setupUi(this);
    screencheck = new ScreenCheck();
    screenresult = new ScreenResult();


    this->ui->stackedWidget->removeWidget(this->ui->page);
    this->ui->stackedWidget->removeWidget(this->ui->page_2);
    this->ui->stackedWidget->addWidget(screencheck);
    this->ui->stackedWidget->addWidget(screenresult);
    this->connect(this->screencheck, &ScreenCheck::tell_window_step_page,this,&MainPage::accept_tell_window_step_page);
    this->connect(this->ui->pushButton,&QPushButton::toggled, this,&MainPage::accept_button_clicked);
    this->connect(this->ui->pushButton_2,&QPushButton::toggled, this,&MainPage::accept_button_clicked);
    this->connect(this->ui->pushButton_3,&QPushButton::toggled, this,&MainPage::accept_button_clicked);
    this->connect(this->screencheck,&ScreenCheck::tell_mainpage_step_page, this, &MainPage::accept_change_page);
}

MainPage::~MainPage()
{
    delete ui;
}

void MainPage::set_current_page(const int &page)
{
    qDebug() << "get";
    if (page == 0){
        this->ui->pushButton->setChecked(true);
            qDebug() << "get1";
    }
    else if (page ==1) {
        this->ui->pushButton_2->setChecked(true);
            qDebug() << "get";
    }
    else if (page ==2) {
        this->ui->pushButton_3->setChecked(true);
    }
}

void MainPage::setMen(const Meninfo &info)
{
    this->screenresult->setMen(info);
}

void MainPage::accept_button_clicked(const bool &checked)
{
    if (checked!=true)
        return;
    else if (this->sender() == this->ui->pushButton) {
        this->ui->stackedWidget->setCurrentIndex(0);
    }
    else if (this->sender() == this->ui->pushButton_2) {
        this->ui->stackedWidget->setCurrentIndex(1);
    }
    else if (this->sender() == this->ui->pushButton_3) {
        this->ui->stackedWidget->setCurrentIndex(2);
    }
}

void MainPage::accept_tell_window_step_page(int page)
{
    emit tell_window_step_page(page);
}

void MainPage::accept_change_page(const int &page)
{
    this->ui->stackedWidget->setCurrentIndex(page);
}
