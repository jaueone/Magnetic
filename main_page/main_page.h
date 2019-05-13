#ifndef MAIN_PAGE_H
#define MAIN_PAGE_H

#include "screencheck.h"
#include "screenresult.h"
#include "login.h"
#include <QWidget>
#include <QPushButton>

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
public slots:
    void accept_button_clicked(const bool &checked);

private slots:

    void accept_tell_window_step_page(int page);

signals:
    void tell_window_step_page(int page);

private:
    Ui::MainPage *ui;
    ScreenCheck *screencheck;
    ScreenResult *screenresult;

};

#endif // MAIN_PAGE_H
