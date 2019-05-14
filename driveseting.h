#ifndef DRIVESETING_H
#define DRIVESETING_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QGridLayout>
#include "my_control.h"

namespace Ui {
class DriveSeting;
}
enum GainAuto{
    Off  = 0,
    Once,
    Continuous
};
enum GammaSelector{
    User = 1,
    sRGB
};
enum ShadingSelector{
    FPNCCorrection = 0,
    PRNUCCorrection
};

struct CameraSetting{
    int thresholdValue_whiteDetect;
    int thresholdValue_blackDetect;

    int width;
    int height;
    int offsetX;
    int offsetY;

    int acquisitionLineRate;
    bool acquisitionLineRateEnable;

    float gain;
    GainAuto gainAuto;

    float gamma;
    bool gammaEnable;
    GammaSelector gammaSelector;

    float exposureTime;
    bool nucEnable;
};

struct SerialSetting {
    QString name;
    int baudRate;
    QString stringBaudRate;
    QSerialPort::DataBits dataBits;
    QString stringDataBits;
    QSerialPort::Parity parity;
    QString stringParity;
    QSerialPort::StopBits stopBits;
    QString stringStopBits;
    QSerialPort::FlowControl flowControl;
    QString stringFlowControl;
    bool localEchoEnabled;
};
class DriveSeting : public QWidget
{
    Q_OBJECT

public:
    explicit DriveSeting(QWidget *parent = nullptr);
    ~DriveSeting();

    SerialSetting get_serial_setting();
    void scan_serial();

private slots:
    void accept_scan_serial();

    void on_pushButton_released();

    void on_serialPortInfoListBox_currentIndexChanged(int index);

    void on_pushButton_7_released();

signals:
    void tell_window_step_page(int page);

private:
    Ui::DriveSeting *ui;
    QSerialPort *serial;

};

#endif // DRIVESETING_H
