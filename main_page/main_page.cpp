#include "main_page.h"
#include "ui_main_page.h"

#include <QDebug>

MainPage::MainPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainPage)
{
    ui->setupUi(this);
    screencheck = new ScreenCheck();
    QWidget *widget = new QWidget(this->ui->stackedWidget);
    this->ui->pushButton_2->hide();
    this->ui->stackedWidget->removeWidget(this->ui->page);
    this->ui->stackedWidget->removeWidget(this->ui->page_2);
    this->ui->stackedWidget->addWidget(screencheck);
    this->ui->stackedWidget->addWidget(widget);
    this->connect(this->screencheck, &ScreenCheck::tell_window_step_page,this,&MainPage::accept_tell_window_step_page);
    this->connect(this->ui->pushButton,&QPushButton::toggled, this,&MainPage::accept_button_clicked);
    this->connect(this->ui->pushButton_2,&QPushButton::toggled, this,&MainPage::accept_button_clicked);
    this->connect(this->ui->pushButton_3,&QPushButton::toggled, this,&MainPage::accept_button_clicked);
    this->connect(this->screencheck,&ScreenCheck::tell_mainpage_step_page, this, &MainPage::accept_change_page);
    this->connect(this->screencheck,&ScreenCheck::tell_window_start_check, this, &MainPage::accept_tell_window_start_check);
}

MainPage::~MainPage()
{
    delete ui;
}

void MainPage::set_current_page(const int &page)
{
    if (page == 0){
        this->ui->pushButton->setChecked(true);
    }
    else if (page ==1) {
        this->ui->pushButton_2->setChecked(true);
    }
    else if (page ==2) {
        this->ui->pushButton_3->setChecked(true);
    }
}

void MainPage::setMen(const Meninfo &info)
{

}

void MainPage::start_check()
{
    this->screencheck->start_check();
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
        this->ui->stackedWidget->setCurrentIndex(1);
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

void MainPage::accept_tell_window_start_check()
{
    emit tell_window_start_check();
}
