#ifndef SCREENCHECK_H
#define SCREENCHECK_H

#include "my_control.h"
#include <QWidget>
#include <QMap>
#include <QString>

namespace Ui {
class ScreenCheck;
}

class ScreenCheck : public QWidget
{
    Q_OBJECT

public:
    explicit ScreenCheck(QWidget *parent = nullptr);
    ~ScreenCheck();

    void addlabel(const QString &name,const QString &content);
private slots:
    void on_pushButton_released();

    void on_pushButton_2_released();

    void on_pushButton_3_released();


    void on_pushButton_4_released();

    void on_pushButton_8_released();

signals:
    void tell_window_step_page(int page);
    void tell_mainpage_step_page(int page);
private:
    Ui::ScreenCheck *ui;
    QMap<QString ,Label *> object_label_map{};
    QString nae;
};

#endif // SCREENCHECK_H
