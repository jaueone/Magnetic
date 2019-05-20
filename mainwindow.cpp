#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "datetime.h"
#include <QDebug>
#include <QDir>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    login = new Login();
    choose = new Choose();
    drivesetting = new DriveSeting();
    mainpage = new MainPage();

    this->setCentralWidget(this->ui->stackedWidget);
    this->ui->stackedWidget->removeWidget(this->ui->page);
    this->ui->stackedWidget->removeWidget(this->ui->page_2);
    this->ui->stackedWidget->addWidget(login);
    this->ui->stackedWidget->addWidget(choose);
    this->ui->stackedWidget->addWidget(drivesetting);
    this->ui->stackedWidget->addWidget(mainpage);
    this->time = new DateTime(this->ui->stackedWidget);
    this->time->setGeometry(920,0,200,50);
    this->sigcon();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sigcon()
{
    this->connect(this->login,&Login::tell_window_step_page,this,&MainWindow::accept_change_page);
    this->connect(this->choose,&Choose::tell_window_step_page,this,&MainWindow::accept_change_page);
    this->connect(this->mainpage,&MainPage::tell_window_step_page, this,&MainWindow::accept_change_page);
    this->connect(this->drivesetting,&DriveSeting::tell_window_step_page, this,&MainWindow::accept_change_page);
    this->connect(this->ui->stackedWidget,&QStackedWidget::currentChanged,this,&MainWindow::on_stackedpage_changed);

    this->connect(this->login,&Login::tell_window_men_login,this,&MainWindow::accept_men_login);
    this->connect(this->choose,&Choose::tell_window_check_self,this,&MainWindow::check_self);

}

void MainWindow::accept_men_login(const Meninfo &info)
{
    this->choose->setMen(info);
    this->mainpage->setMen(info);
}

void MainWindow::accept_change_page(const int &page)
{
    if (page == 0){
        Meninfo info;
        this->login->clear();
        this->choose->setMen(info);
        this->ui->stackedWidget->setCurrentIndex(0);
    }
    else if(page < 4)
        this->ui->stackedWidget->setCurrentIndex(page);
    else if (page >= 4) {
        this->ui->stackedWidget->setCurrentIndex(3);
        this->mainpage->set_current_page(page - 3);
    }
}

void MainWindow::on_stackedpage_changed(const int &page)
{
    if (page == 0){
        this->time->changedstyle("white");
    }
    else {
        this->time->changedstyle("black");
    }
}

void MainWindow::check_self()
{
    this->drivesetting->check_self();
}


