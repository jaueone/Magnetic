#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QThread>
#include <QtSerialPort>
#include <QString>
#include <QTimer>
//#include <memory.h>
//#include <QDebug>

//typedef unsigned char           uint8_t;
//typedef unsigned short int      uint16_t;
//typedef unsigned int            uint32_t;

//enum Command:short
//{
//    QueryStatus = 0x0000,
//    StartWork ,
//    CheckResult,
//    StopWork,
//    Reset,
//    SetTransportMotorSpeed,
//    SetRollMontorSpeed,
//    SetRollerMotorSpeed,
//    SetSlidingTableMotorSpeed,
//};
//struct Package
//{
//    uint16_t head;                        //命令
//    uint8_t packageNumber;                //帧序号
//    uint8_t packageLength;                //长度
//    uint8_t commandCount;                 //命令个数
//    uint16_t command;                     //命令
//    uint8_t length;                       //长度
//    uint8_t *pBuffer;                     //数据
//    uint16_t crc;
//};

//QByteArray dumpData(const Command &command,unsigned char* data,const uint8_t &len){

//    QByteArray ba;
//    Package package;
//    package.head = 0xFFDD;
//    package.packageNumber = 0x00;
//    package.packageLength = 10 + len;
//    package.commandCount = 1;
//    package.command = (uint16_t)command;
//    package.length = len;
//    package.pBuffer = data;

//    memcpy(ba.data(),&package,sizeof(package));
//    qDebug() << sizeof (package);
//    return ba;
//};

class Worker:public QThread
{
    Q_OBJECT
public:
    Worker(QSerialPort *serial){
        this->serial = serial;
        this->timer = new QTimer(this);
        this->connect(this->serial, &QSerialPort::readyRead,this, &Worker::accept_read_data);
        this->connect(this->serial, &QSerialPort::errorOccurred,this, &Worker::accept_serial_error);
        this->connect(this->timer, &QTimer::timeout, this,&Worker::accept_timeout);
    }

    void run() override;

    void query_status();
    void start_work();
    void wait_check_command();
    void tell_stm_result();
    void wait_wrap_result();

signals:
    void tell_window_message(QString type, QString msg);

public slots:
    void accept_read_data();
    void accept_serial_error(QSerialPort::SerialPortError error);
    void accept_timeout();

private:
    QSerialPort *serial;
    QTimer *timer;
    int work_step = 0;
    bool stop_work = false;

};

#endif // WORKER_H
