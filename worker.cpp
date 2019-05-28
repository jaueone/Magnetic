#include "worker.h"

void Worker::run()
{
    this->work_step = 0;
    this->stop_work = false;
    while (!stop_work) {
        switch (work_step) {
        case 0:
            this->query_status();
            this->work_step = 1;
            break;
        case 1:
            this->start_work();
            this->work_step = 2;
            break;
        case 2:
            this->wait_check_command();
            this->work_step = 3;
            break;
        case 3:
            this->tell_stm_result();
            this->work_step = 4;
            break;
        case 4:
            this->wait_wrap_result();
            this->work_step = 0;
            break;
        }
    }
}

void Worker::query_status()
{

}

void Worker::start_work()
{

}

void Worker::wait_check_command()
{

}

void Worker::tell_stm_result()
{

}

void Worker::wait_wrap_result()
{

}


void Worker::accept_read_data()
{
    this->serial->readAll();
}

void Worker::accept_serial_error(QSerialPort::SerialPortError error)
{

}

void Worker::accept_timeout()
{
    this->timer->stop();
    this->stop_work = true;
}
