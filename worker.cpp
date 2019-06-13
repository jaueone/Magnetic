#include "worker.h"
#include "serial.h"

#define TimeOut 120000

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
    this->connect(this->serial,&QSerialPort::readyRead,this,&Worker::accept_read_data);
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
    timer->start(TimeOut);
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
    timer->start(TimeOut);
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
    char c;
    if (this->quality < 9)
        c = 0x01;
    else
        c = 0x00;
    data.append(c);
    serial->write(Worker::dump_data(Command::ImageCapture,data));
    if (this->quality >= 9)
        this->quality = 0;
    this->quality += 1;
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
{   QByteArray ba(data);
    QDataStream in(&ba,QIODevice::ReadOnly);
    status.status = (uint8_t)data.at(0);
    status.E_Stop = (status.status & 0x01) == 0x01 ? true : false;
    status.transportMotorSpeedStatus = (status.status & 0x02) == 0x02 ? true : false;
    status.rollMontorSpeedStatus = (status.status & 0x04) == 0x04 ? true : false;
    status.rollerMotorSpeedStatus = (status.status & 0x08) == 0x08 ? true : false;
    status.slidingTableMotorSpeedStatus = (status.status & 0x10) == 0x10 ? true : false;
    status.isWorking = (status.status & 0x20) == 0x20 ? true : false;
    status.isReseting = (status.status & 0x40) == 0x40 ? true : false;
    in >> status.status >> status.transport_motor_speed >> status.roll_motor_speed >> status.roller_motor_speed >> status.slidingtable_motor_speed;
    qDebug() << status.E_Stop << status.transportMotorSpeedStatus << status.rollMontorSpeedStatus << status.rollerMotorSpeedStatus << status.slidingTableMotorSpeedStatus << status.isWorking << status.isReseting
             << status.transport_motor_speed << status.roll_motor_speed << status.roller_motor_speed << status.slidingtable_motor_speed;
    this->is_Stoped_Work = status.isReseting;
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

void Worker::set_motor_speed(Command com, uint16_t data)
{
    if(this->is_Stoped_Work)
        return;
    if(!serial->isOpen())
        return;
    if (this->step != -1)
        return;
    QByteArray da;
    QDataStream d(&da,QIODevice::ReadWrite);
    if (com == 1){
        d << status_speed.transport_motor_speed;
        serial->write(Worker::dump_data(Command::SetTransportMotorSpeed,da));
    }
    else if (com == 2){
        d << data;
        serial->write(Worker::dump_data(Command::SetRollMontorSpeed,da));
        }
    else if (com == 3){
        d << data;
        serial->write(Worker::dump_data(Command::SetRollerMotorSpeed,da));
    }
    else if (com == 4){
        d << data;
        serial->write(Worker::dump_data(Command::SetSlidingTableMotorSpeed,da));
    }
    timer->start(TimeOut);
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
            emit tell_window_command(Command::ImageCapture,0);
            step = 3;
            emit tell_window_work_step(step);
            qDebug() << "send_get_check_command";
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
                emit tell_window_work_step(step);
                start_work();
                qDebug() << "start_work";
            }
            else if(step == 1){
                step = 2;
                emit tell_window_work_step(step);
                qDebug() << "wait_check_command";
            }
            else if (step == 3) {
                step  = 4;
                emit tell_window_work_step(step);
                qDebug() << "wait_wrap_result";
            }
            else if (step == -1) {
                qDebug() << "stop stm";
            }
            else if (step == -2) {
                qDebug() << "reset stm";
            }
            else if (step == -3) {
                qDebug() << "set speed";
            }
            break;

        case WrapResult:
            if (step != 4)
                return;
            response_wrap_result(resp.data);
            emit tell_window_command(Command::WrapResult,(int)resp.data[0]);
            this->step = 5;
            emit tell_window_work_step(step);
            sleep(8000);
            this->step = 0;
            emit tell_window_work_step(step);
            sleep(10);
            query_status();
            qDebug() << "next unit query_status";
            break;

        case RespondOK:
            qDebug() << "set speed ok";
            break;
    }
}

void Worker::accept_command_to_stm(Command com, int data)
{
    QByteArray da;
    QDataStream stream(&da,QIODevice::WriteOnly);
    if(this->is_Stoped_Work)
        return;
    if(!serial->isOpen())
        return;
    if (com == Command::StopWork){
        stream << (uint8_t)data;
        this->is_Stoped_Work = true;
        step = -1;
    }
    else if (com ==Command::Reset){
        stream << (uint8_t)data;
        this->is_Stoped_Work = false;
        step = -2;
    }
    else if(com ==Command::SetRollMontorSpeed ||com ==Command::SetRollerMotorSpeed ||com ==Command::SetTransportMotorSpeed ||com ==Command::SetSlidingTableMotorSpeed){
        stream << (uint16_t)data;
        step = -3;
    }
    else if (com == Command::CheckResult) {
        if (step != 3)
            return;
        stream << (uint8_t)data;
        qDebug() << "tell_stm_result";
    }
    serial->write(Worker::dump_data(com,da));
    timer->start(TimeOut);

}

void Worker::accept_stop_work()
{
    this->is_Stoped_Work = true;
    if (this->serial->isOpen())
        this->serial->close();
}

void Worker::accept_open_serial(SerialSetting setting)
{

    if (serial->isOpen()){
        serial->close();
    }
    this->setting = setting;
    serial->setPortName(setting.name);
    serial->setBaudRate(setting.baudRate);
    serial->setDataBits(setting.dataBits);
    serial->setParity(setting.parity);
    serial->setStopBits(setting.stopBits);
    serial->setFlowControl(setting.flowControl);
    emit tell_window_serial_status(serial->open(QIODevice::ReadWrite));

}

void Worker::accept_timeout()
{
    timer->stop();
    this->is_Stoped_Work = true;
    emit tell_window_stm_respond_timeout();
    qDebug() << "timeout";
}


void Worker::accept_serial_error(const QSerialPort::SerialPortError &error)
{
    if (error == QSerialPort::NoError)
        return;
    if (serial->isOpen())
        serial->close();
    timer->stop();
    emit tell_window_serial_status(false);
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
    emit tell_window_work_step(0);
    this->is_Stoped_Work = false;
    this->query_status();
}


