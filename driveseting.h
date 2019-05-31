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

    void scan_serial();

    void save_setting(SerialSetting setting_ser,CameraSetting setting_cam);

    unsigned short int CRC16(unsigned char* pchMsg, unsigned short int wDataLen);

    void check_self();

    QSerialPort *Serial(){ return serial;}

    HKCamera * Camera(){ return camera;}


private slots:
    void accept_scan_serial();

    void accept_stm_status(const Status &status);

    void on_pushButton_released();

    void on_serialPortInfoListBox_currentIndexChanged(int index);

    void on_pushButton_7_released();

    void on_pushButton_2_released();

    void on_pushButton_4_released();

    void on_pushButton_9_released();

    void on_pushButton_11_clicked();

    void on_pushButton_10_clicked();

    void on_spinBox_3_valueChanged(int arg1);

    void on_pushButton_12_clicked();

    void on_pushButton_13_clicked();

    void on_pushButton_14_clicked();

    void on_pushButton_15_released();

signals:
    void tell_window_step_page(int);
    void tell_worker_setting(SerialSetting);
    void tell_worker_set_motor_speed(Status);

private:
    Ui::DriveSeting *ui;
    QSerialPort *serial;
    HKCamera *camera;

    WorkerThread *worker_thread;
    bool Serial_OK;
    bool Camera_OK;
};

#endif // DRIVESETING_H
