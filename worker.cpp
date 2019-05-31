#include "worker.h"
#include "serial.h"

void sleep(unsigned int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void Worker::init_setting_and_connect()
{
    serial->setPortName(setting.name);
    serial->setBaudRate(setting.baudRate);
    serial->setDataBits(setting.dataBits);
    serial->setParity(setting.parity);
    serial->setStopBits(setting.stopBits);
    serial->setFlowControl(setting.flowControl);
    this->connect(this->serial,&QSerialPort::readyRead, this,&Worker::accept_read_data);
    this->connect(this->timer,&QTimer::timeout,this,&Worker::accept_timeout);
    this->connect(this->serial,&QSerialPort::errorOccurred, this,&Worker::accept_serial_error);
}

void Worker::query_status()
{
    if(this->is_Stoped_Work)
        return;
    if(!serial->isOpen())
        return;
    if (this->step != 0)
        return;
    QByteArray data;
    char c = 0x00;
    data.append(c);
    serial->write(Worker::dump_data(Command::QueryStatus,data));
    qDebug() <<"send query"<< Worker::dump_data(Command::QueryStatus,data).toHex();
    timer->start(12000);
}

void Worker::start_work()
{
    if(this->is_Stoped_Work)
        return;
    if(!serial->isOpen())
        return;
    if (this->step != 1)
        return;
    QByteArray data;
    char c = 0x00;
    data.append(c);
    serial->write(Worker::dump_data(Command::StartWork,data));
    timer->start(12000);
}

void Worker::response_check_command()
{
    if(this->is_Stoped_Work)
        return;
    if(!serial->isOpen())
        return;
    if (this->step != 2)
        return;
    QByteArray data;
    char c = 0x00;
    data.append(c);
    serial->write(Worker::dump_data(Command::ImageCapture,data));
}

void Worker::tell_stm_result()
{
    if(this->is_Stoped_Work)
        return;
    if(!serial->isOpen())
        return;
    if (this->step != 3)
        return;
    QByteArray data;
    char c = 0x01;
    data.append(c);
    serial->write(Worker::dump_data(Command::CheckResult,data));
    timer->start(12000);
}

void Worker::response_wrap_result(const QByteArray &data)
{
    if(this->is_Stoped_Work)
        return;
    if(!serial->isOpen())
        return;
    if (this->step != 4)
        return;
    serial->write(Worker::dump_data(Command::WrapResult,data));
}

void Worker::analysis_MCStatus(const QByteArray &data)
{
    status.status = (uint8_t)data.at(0);
    status.E_Stop = (status.status & 0x01) == 0x01 ? true : false;
    status.transportMotorSpeedStatus = (status.status & 0x02) == 0x02 ? true : false;
    status.rollMontorSpeedStatus = (status.status & 0x03) == 0x03 ? true : false;
    status.rollerMotorSpeedStatus = (status.status & 0x04) == 0x04 ? true : false;
    status.slidingTableMotorSpeedStatus = (status.status & 0x05) == 0x05 ? true : false;
    status.isWorking = (status.status & 0x06) == 0x06 ? true : false;

    status.transport_motor_speed = data.mid(1,2).toUShort();
    status.roll_motor_speed = data.mid(3,2).toUShort();
    status.roller_motor_speed= data.mid(5,2).toUShort();
    status.slidingtable_motor_speed = data.mid(7,2).toUShort();
    emit tell_window_stm_status(status);
}

Respond Worker::load_data(const QByteArray &data)
{
    Respond respond;
    if (data.length() <10)
         return respond;
    QByteArray ba(data);
    Package package;
    QDataStream in(&ba,QIODevice::ReadOnly);
    in >> package.head >> package.packageNumber >>package.packageLength >> package.commandCount >>package.command >> package.length;
    if (package.head != 0xFDFD)
        return respond;
    if(package.packageLength != data.size())
        return respond;
    if (package.command !=0){
        Command com = static_cast<Command>(package.command);
        if (!com) return respond;
    }
    uint8_t d;
    QByteArray data_;
    for(int i=0;i<package.length;i++){
        in >> d;
        data_.append((char)d);
    }
    in >> package.crc;
    uint16_t crc=0;
    for (int i = 0; i < 8+package.length; i++) {
        crc += (uint8_t)data.at(i);
    }
    if (crc != package.crc)
        return respond;
    respond.isGood = true;
    respond.command = (Command)package.command;
    respond.data = data_;
    return respond;
}

QByteArray Worker::dump_data(const Command &command,const QByteArray &data)
{
    QByteArray ba;
    QDataStream out(&ba,QIODevice::WriteOnly);
    uint8_t len = (uint8_t)data.length();
    Package package;
    package.head = 0xFDFD;
    package.packageNumber = 0x00;
    package.packageLength = 10 + len;
    package.commandCount = 1;
    package.command = (uint16_t)command;
    package.length = len;
    out << package.head << package.packageNumber << package.packageLength << package.commandCount <<package.command << package.length;
    for (auto d:data){
        out << (uint8_t)d;
    }
    for(auto c:ba){
        package.crc += (uint8_t)c;
    }
    out << package.crc;
    return ba;
}

void Worker::accept_read_data()
{
    QByteArray data = this->serial->readAll();
    Respond resp = Worker::load_data(data);
    if (!resp.isGood)
        return;
    timer->stop();
    qDebug() << (uint8_t)resp.command << resp.data.toHex();

    switch (resp.command) {

        case ImageCapture:
            if (step != 2)
                return;
            response_check_command();
            qDebug() << "send_get_check_command";
            step = 3;
            sleep(10);
            tell_stm_result();
            qDebug() << "tell_stm_result";
            break;

        case STM_WorkStatus:
            this->analysis_MCStatus(resp.data);
            if (status.E_Stop){
                this->is_Stoped_Work = true;
                return;
            }
            if (status.rollMontorSpeedStatus ||status.rollerMotorSpeedStatus || status.transportMotorSpeedStatus || status.slidingTableMotorSpeedStatus){
                this->is_Stoped_Work = true;
                return;
            }
            if (step == 0){
                step = 1;
                start_work();
                qDebug() << "start_work";
            }
            else if(step == 1){
                step = 2;
                qDebug() << "wait_check_command";
            }
            else if (step == 3) {
                step  = 4;
                qDebug() << "wait_wrap_result";
            }
            break;

        case WrapResult:
            if (step == 4){
                  response_wrap_result(resp.data);
                  this->step = 0;
                  sleep(10);
                  query_status();
                  qDebug() << "next unit query_status";
            }
            break;
    }
}

void Worker::accept_timeout()
{
    timer->stop();
    this->is_Stoped_Work = true;
}


void Worker::accept_serial_error(const QSerialPort::SerialPortError &error)
{
    if (error == QSerialPort::NoError)
        return;
    timer->stop();
    this->is_Stoped_Work = true;
    qDebug() << "serial errorOccurred" << error;
}

void Worker::accept_serial_setting(SerialSetting setting)
{
    if (serial->isOpen())
        serial->close();
    this->setting = setting;
    serial->setPortName(setting.name);
    serial->setBaudRate(setting.baudRate);
    serial->setDataBits(setting.dataBits);
    serial->setParity(setting.parity);
    serial->setStopBits(setting.stopBits);
    serial->setFlowControl(setting.flowControl);
    if (!serial->open(QIODevice::ReadWrite)){
        return;
    }
    step = 0;
    this->query_status();
}

void Worker::accept_set_motor_speed(const Status &status)
{
    if(this->is_Stoped_Work)
        return;
    if(!serial->isOpen())
        return;
    if (this->step != 0)
        return;
    QByteArray data;
    char c = 0x00;
    data.append(c);
    serial->write(Worker::dump_data(Command::QueryStatus,data));

}
