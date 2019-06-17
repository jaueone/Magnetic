#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "datetime.h"
#include "login.h"
#include "choose.h"
#include "driveseting.h"
#include "main_page/main_page.h"
#include "worker.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void sigcon();
    void initdir();

public slots:
    void accept_men_login(const Meninfo &info);
    void accept_change_page(const int &page);
    void accept_get_picture();
    void accept_stm_staus(const Status &status);


    void accept_camera_start_check();
    void check_self();

private:
    Ui::MainWindow *ui;
    DateTime *time;
    Login *login;
    Choose *choose;
    MainPage *mainpage;
    DriveSeting *drivesetting;
    ScreenCheck *screen_check;
    ScreenResult *screen_result;

    HKCamera *camera;
    QSerialPort *serial;
    QLabel *camera_label_;

    QScrollArea widget;
    QLabel label;
    DefectsDetect detect;

    Worker *worker;
signals:
    void camera_start_preview();

private slots:
    void on_stackedWidget_currentChanged(int arg1);
};

#endif // MAINWINDOW_H
