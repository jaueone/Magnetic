#ifndef DRIVESETING_H
#define DRIVESETING_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>

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
    ShadingSelector ShadingSelector;
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

private slots:
    void on_pushButton_released();
    void on_pushButton_6_released();

    void on_pushButton_5_released();



    void on_serialPortInfoListBox_currentIndexChanged(int index);

signals:
    void tell_window_step_page(int page);

private:
    Ui::DriveSeting *ui;
    QSerialPort *serial;
};

#endif // DRIVESETING_H
