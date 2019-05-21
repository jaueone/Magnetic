#ifndef MAIN_PAGE_H
#define MAIN_PAGE_H

#include "screencheck.h"
#include "screenresult.h"
#include "login.h"
#include <QWidget>
#include <QPushButton>
#include <QSerialPort>

namespace Ui {
class MainPage;
}

class MainPage : public QWidget
{
    Q_OBJECT

public:
    explicit MainPage(QWidget *parent = nullptr);
    ~MainPage();
    void set_current_page(const int &page);

    void setMen(const Meninfo &info);
    void start_check();
    void setSerial(QSerialPort* serial);

    ScreenCheck *ScreenCheck_(){return this->screencheck;}
public slots:
    void accept_button_clicked(const bool &checked);

private slots:

    void accept_tell_window_step_page(int page);
    void accept_change_page(const int &page);
    void accept_tell_window_start_check();

signals:
    void tell_window_step_page(int page);
    void tell_window_start_check();
private:
    Ui::MainPage *ui;
    ScreenCheck *screencheck;

};

#endif // MAIN_PAGE_H
