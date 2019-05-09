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
    mainpage = new MainPage();
    this->setCentralWidget(this->ui->stackedWidget);
    this->ui->stackedWidget->removeWidget(this->ui->page);
    this->ui->stackedWidget->removeWidget(this->ui->page_2);
    this->ui->stackedWidget->addWidget(login);
    this->ui->stackedWidget->addWidget(choose);
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
    this->connect(this->login,&Login::tell_window_step_page2,this,&MainWindow::accept_change_page);
    this->connect(this->choose,&Choose::tell_window_step_page3,this,&MainWindow::accept_change_page);
    this->connect(this->ui->stackedWidget,&QStackedWidget::currentChanged,this,&MainWindow::on_stackedpage_changed);

}

void MainWindow::accept_change_page(const int &page)
{
    if(page < 3)
        this->ui->stackedWidget->setCurrentIndex(page);
    else if (page >= 3) {
        this->ui->stackedWidget->setCurrentIndex(2);
        this->mainpage->set_current_page(page - 2);
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


