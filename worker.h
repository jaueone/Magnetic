#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QThread>
#include <QtSerialPort>
#include <QString>
#include <QTimer>
#include <QDebug>
#include <QList>
#include "camera.h"
#include "serial.h"

typedef unsigned char           uint8_t;
typedef unsigned short int      uint16_t;
typedef unsigned int            uint32_t;

enum Command:short
{
    QueryStatus = 0x0000,
    StartWork = 0x0001,
    CheckResult = 0x0002,
    StopWork = 0x0003,
    Reset = 0x0004,
    SetTransportMotorSpeed = 0x0005,
    SetRollMontorSpeed = 0x0006,
    SetRollerMotorSpeed = 0x0007,
    SetSlidingTableMotorSpeed = 0x0008,

    ImageCapture = 0x00FE,
    STM_WorkStatus = 0X00FD,
    WrapResult = 0X00FC,
};

struct Package
{
    uint16_t head;                        //命令
    uint8_t packageNumber;                //帧序号
    uint8_t packageLength;                //长度
    uint8_t commandCount;                 //命令个数
    uint16_t command;                     //命令
    uint8_t length;                       //长度
    uint8_t *pBuffer;                     //数据
    uint16_t crc = 0;
};

struct Respond{
    bool isGood = false;
    Command command;
    QByteArray data;
};

struct Status{
    uint8_t status;
    bool E_Stop;
    bool transportMotorSpeedStatus;
    bool rollMontorSpeedStatus;
    bool rollerMotorSpeedStatus;
    bool slidingTableMotorSpeedStatus;
    bool isWorking;
    uint16_t transport_motor_speed;
    uint16_t roll_motor_speed;
    uint16_t roller_motor_speed;
    uint16_t slidingtable_motor_speed;
};

class Worker:public QObject
{
    Q_OBJECT

public:
    Worker(){
        this->serial = new QSerialPort(this);
        this->timer = new QTimer(this);
        this->init_setting_and_connect();
    }
    ~Worker(){}

    void init_setting_and_connect();
    void query_status();
    void start_work();
    void response_check_command();
    void tell_stm_result();
    void response_wrap_result(const QByteArray &data);

    void analysis_MCStatus(const QByteArray &data);

    static QByteArray dump_data(const Command &command,const QByteArray &data);
    static Respond load_data(const QByteArray &data);

signals:
    void tell_window_message(QString type, QString msg);
    void tell_window_stm_status(Status);

public slots:
    void accept_read_data();
    void accept_timeout();
    void accept_serial_error(const QSerialPort::SerialPortError &error);
    void accept_serial_setting(SerialSetting setting);
    void accept_set_motor_speed(const Status &status);

private:
    int step = 0;

    bool is_Stoped_Work = false;


    QTimer *timer;
    QSerialPort *serial;
    Status status;
    SerialSetting setting;
};

class WorkerThread: public QThread
{
    Q_OBJECT

public:
    ~WorkerThread(){
        this->quit();
        this->wait();
    }

    void run(){
        Worker worker;
        this->connect(this, &WorkerThread::accept_setting, &worker,&Worker::accept_serial_setting);
        this->connect(this, &WorkerThread::tell_window_stm_status, &worker,&Worker::tell_window_stm_status);
        this->connect(this, &WorkerThread::tell_work_set_motor_speed, &worker,&Worker::accept_set_motor_speed);
        this->exec();
    }

signals:
    void accept_setting(SerialSetting setting);
    void tell_window_stm_status(Status);
    void tell_work_set_motor_speed(Status);

};

#endif // WORKER_H
