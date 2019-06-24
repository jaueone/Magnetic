#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "datetime.h"
#include "camera.h"
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QFileDialog>
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
    screen_result = mainpage->ScreenResult_();
    camera_label_ = screen_check->camera_label();

    camera = drivesetting->Camera();
    serial = drivesetting->Serial();
    worker = this->screen_check->workerObj();


   //this->worker_thread = new Worker(serial);
   //this->screen_check->setWorkerThread(worker_thread);
    this->screen_check->setSerial(serial);
    this->screen_check->setCamera(camera);
    this->screen_result->setSerial(serial);

    this->time = new DateTime(this->ui->stackedWidget);
    this->time->setGeometry(860,0,200,50);

    this->setCentralWidget(this->ui->stackedWidget);

    this->ui->stackedWidget->removeWidget(this->ui->page);
    this->ui->stackedWidget->removeWidget(this->ui->page_2);

    this->ui->stackedWidget->addWidget(login);
    this->ui->stackedWidget->addWidget(choose);
    this->ui->stackedWidget->addWidget(drivesetting);
    this->ui->stackedWidget->addWidget(mainpage);


    this->sigcon();
    this->initdir();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sigcon()
{
    this->connect(this->login,&Login::tell_window_step_page,this,&MainWindow::accept_change_page);
    this->connect(this->choose,&Choose::tell_window_step_page,this,&MainWindow::accept_change_page);
    this->connect(this->screen_check,&ScreenCheck::tell_window_step_page, this,&MainWindow::accept_change_page);
    this->connect(this->drivesetting,&DriveSeting::tell_window_step_page, this,&MainWindow::accept_change_page);

    this->connect(this->login,&Login::tell_window_men_login,this,&MainWindow::accept_men_login);
    this->connect(this->choose,&Choose::tell_window_check_self,this,&MainWindow::check_self);
    this->connect(this->choose,&Choose::tell_window_get_picture,this,&MainWindow::accept_get_picture);
    this->connect(this->screen_check,&ScreenCheck::tell_window_start_check, this,&MainWindow::accept_camera_start_check);
    this->connect(this->screen_check,&ScreenCheck::tell_window_stm_status, this->drivesetting,&DriveSeting::accept_stm_status);
    this->connect(this->screen_check,&ScreenCheck::ask_serial_setting, this->drivesetting,&DriveSeting::accept_return_serial_setting);
    this->connect(this->drivesetting,&DriveSeting::tell_screencheck_setting,this->screen_check,&ScreenCheck::accept_serial_setting);

//    this->connect(this->drivesetting,&DriveSeting::tell_worker_stm_command,this->worker,&Worker::accept_command_to_stm,Qt::QueuedConnection);
//    this->connect(this->drivesetting,&DriveSeting::tell_worker_stop_work,this->worker,&Worker::accept_stop_work,Qt::QueuedConnection);
//    this->connect(this->drivesetting,&DriveSeting::tell_worker_open_serial,this->worker,&Worker::accept_open_serial,Qt::QueuedConnection);
//    this->connect(this->worker,&Worker::tell_window_serial_status,this->drivesetting,&DriveSeting::accept_serial_status,Qt::QueuedConnection);
//    this->connect(this->worker,&Worker::tell_window_command,this->drivesetting,&DriveSeting::accept_stm_command,Qt::QueuedConnection);
//   this->connect(this->worker,&Worker::tell_window_stm_respond_timeout,this->drivesetting,&DriveSeting::accept_stm_respond_timeout,Qt::QueuedConnection);
}

void MainWindow::initdir()
{
    QDir hobject("./database/hobject/");
    if(hobject.exists()){
    }
    else {
        bool ok = hobject.mkdir("./database/hobject/");
    }
}

void MainWindow::accept_men_login(const Meninfo &info)
{
    this->choose->setMen(info);
    this->screen_check->setMen(info);
    this->screen_result->setMen(info);
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


void MainWindow::accept_get_picture()
{
//    QString filename;
//    filename=QFileDialog::getOpenFileName(this, tr("选择图像"), "", tr("Images (*.png *.bmp *.jpg *.tif *.GIF )"));
//    if(filename.isEmpty())
//        return;

//    widget.setWidget(&label);
//    widget.resize(610,510);
//    label.resize(600,510);


//    HObject ho_Image;
//    ReadImage(&ho_Image, HTuple(filename.toStdString().c_str())); // 此方法Halcon提供
//    Hlong winID =(Hlong)widget.winId();

    //    widget.show();
}

void MainWindow::accept_stm_staus(const Status &status)
{
    this->drivesetting->accept_stm_status(status);
}

void MainWindow::check_self()
{
    if (this->drivesetting->init()){
       this->camera->startCollect();
       this->camera->collectFrame(this->camera_label_);
       this->ui->stackedWidget->setCurrentIndex(3);
       this->screen_check->start_check();
    }
    else
        this->drivesetting->display_init();
}

void MainWindow::accept_camera_start_check()
{
    if (this->drivesetting->init()){
        this->screen_check->start_check();
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

void MainWindow::on_stackedWidget_currentChanged(int arg1)
{
    if (arg1 == 0){
        this->time->changedstyle("white");
    }
    else {
        this->time->changedstyle("black");
    }
}
