#ifndef SCREENCHECK_H
#define SCREENCHECK_H

#include "my_control.h"
#include "camera.h"
#include <QWidget>
#include <QMap>
#include <QString>
#include <QtSerialPort>
#include"login.h"

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
    void start_check();
    void setMen(const Meninfo &info);
    void setSerial(QSerialPort* serial_);
    void setCamera(HKCamera *camera_);
    void serial_send_start();
    void serial_send_end();
    void serial_send_change_image();
    void save_check_result(const QString &result,const QString &defect, const QString &time, const QString &num, const QString &name);
    QLabel * camera_label();
private slots:
    void on_pushButton_released();

    void on_pushButton_4_released();

    void on_pushButton_8_released();

    void on_lcd_out_clicked(bool checked);
    void on_lcd_change_image_released();
    void on_start_check_released();

signals:
    void tell_window_step_page(int page);
    void tell_window_start_check();

private:
    Ui::ScreenCheck *ui;
    QMap<QString ,Label *> object_label_map{};
    QSerialPort *serial;
    HKCamera *camera;
    Meninfo men_info;
    unsigned char id= 1;
};

#endif // SCREENCHECK_H
