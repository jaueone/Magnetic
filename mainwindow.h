#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "datetime.h"
#include "login.h"
#include "choose.h"
#include "driveseting.h"
#include "main_page/main_page.h"
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

public slots:
    void accept_men_login(const Meninfo &info);
    void accept_change_page(const int &page);
    void on_stackedpage_changed(const int &page);


    void check_self();

private:
    Ui::MainWindow *ui;
    DateTime *time;
    Login *login;
    Choose *choose;
    MainPage *mainpage;
    DriveSeting *drivesetting;
};

#endif // MAINWINDOW_H
