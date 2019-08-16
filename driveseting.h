#ifndef DRIVESETING_H
#define DRIVESETING_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QGridLayout>
#include "my_control.h"
#include "camera.h"
#include "serial.h"
#include "worker.h"

namespace Ui {
class DriveSeting;
}

class DriveSeting : public QWidget
{
    Q_OBJECT

public:
    explicit DriveSeting(QWidget *parent = nullptr);
    ~DriveSeting();

    bool init();

    void display_init();

    QString init_camera();

    QString init_serial();

    SerialSetting get_serial_setting();

    CameraSetting get_camera_setting();

    void load_setting();

    void load_algorithm();

    QJsonObject get_algorithm();

    void scan_serial();

    void save_setting(SerialSetting setting_ser,CameraSetting setting_cam);

    unsigned short int CRC16(unsigned char* pchMsg, unsigned short int wDataLen);

    void check_self();

    void set_camera_params();

    QSerialPort *Serial(){ return serial;}

    HKCamera * Camera(){ return camera;}



public slots:
    void accept_scan_serial();
    void accept_stm_status(const Status &status);
    void accept_stm_respond_timeout();
    void accept_return_serial_setting(){emit tell_screencheck_setting(this->get_serial_setting());}
    void accept_serial_status(bool isopened);
    void accept_stm_command(Command com,int data);

signals:
    void tell_window_step_page(int);
    void tell_screencheck_setting(SerialSetting);
    void tell_worker_stm_command(Command com,int data);
    void tell_worker_open_serial(SerialSetting);
    void tell_worker_stop_work();

private slots:
    void on_pushButton_released();

    void on_serialPortInfoListBox_currentIndexChanged(int index);

    void on_pushButton_2_released();

    void on_pushButton_4_released();

    void on_pushButton_7_clicked(bool checked);

    void on_pushButton_9_released();

    void on_pushButton_11_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_15_released();

    void on_pushButton_16_released();

    void on_pushButton_19_released();

    void on_pushButton_20_released();

    void on_pushButton_13_released();

    void on_pushButton_14_released();

    void on_pushButton_17_released();

    void on_pushButton_18_released();


    void on_pushButton_5_released();

    void on_pushButton_6_released();

private:
    Ui::DriveSeting *ui;
    QSerialPort *serial;
    HKCamera *camera;

    bool Serial_OK;
    bool Camera_OK;

    bool worker_serial_status;
};

#endif // DRIVESETING_H
