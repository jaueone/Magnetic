#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "datetime.h"
#include "camera.h"
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QTime>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    login = new Login();
    choose = new Choose();
    drivesetting = new DriveSeting();
    mainpage = new MainPage();

    screen_check = mainpage->ScreenCheck_();
    camera_label_ = screen_check->camera_label();

    camera = drivesetting->Camera();
    serial = drivesetting->Serial();

    preview_thread = new PreviewThread(camera,camera_label_);
    collect_thread = new CollectThread(camera,camera_label_);

    this->screen_check->setSerial(serial);
    this->screen_check->setCamera(camera);
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
    delete preview_thread;
    delete collect_thread;
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
    this->connect(this->mainpage,&MainPage::tell_window_start_check, this,&MainWindow::accept_camera_start_check);
    this->connect(this->screen_check,&ScreenCheck::tell_window_kill_preview_thread, this,&MainWindow::accept_kill_preview_thread);
    //this->connect(this,&MainWindow::camera_start_preview, this->preview_thread,&QThread::start);
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
    QTime start = QTime::currentTime();

    if (this->drivesetting->init()){
       QTime end =  QTime::currentTime();
       qDebug() << "end";
       this->preview_thread->run();
       this->ui->stackedWidget->setCurrentIndex(3);
    }
    else
        this->drivesetting->display_init();
}

void MainWindow::accept_camera_start_check()
{
    if (this->drivesetting->init()){
        this->mainpage->start_check();
    }
    else {
        QMessageBox messageBox;
        messageBox.setWindowTitle("警告");
        messageBox.setIcon(QMessageBox::Warning);
        QPushButton button("确定");
        messageBox.setText("系统自检失败,\n请先返回选择页面完成系统自检!");
        messageBox.exec();
    }
}

void MainWindow::accept_kill_preview_thread()
{
    this->preview_thread->quit();
}




