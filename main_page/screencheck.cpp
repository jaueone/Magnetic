#include "screencheck.h"
#include "ui_screencheck.h"
#include "DefectsDetect.h"
#include "my_control.h"
#include <QSqlError>
#include <QDateTime>



void msleep(unsigned int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

const unsigned char chCRCHTalbe[] =
{
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,// CRC 高位字节值表
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40
};

const unsigned char chCRCLTalbe[] =
{
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,// CRC 低位字节值表
0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,
0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
0x41, 0x81, 0x80, 0x40
};

unsigned short int CRC16(unsigned char *pchMsg, unsigned short int wDataLen)
{
    unsigned char chCRCHi = 0xFF;
    unsigned char chCRCLo = 0xFF;
    short wIndex;

       while (wDataLen--)
       {
           // 计算CRC
           wIndex = chCRCLo ^ *pchMsg++ ;
           chCRCLo = chCRCHi ^ chCRCHTalbe[wIndex];
           chCRCHi = chCRCLTalbe[wIndex] ;
       }

       return ((chCRCHi << 8) | chCRCLo) ;
}



ScreenCheck::ScreenCheck(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScreenCheck)
{
    ui->setupUi(this);
    qimage = new QImage;
    this->worker = new Worker;
    this->worker_thread = new QThread;
    worker->moveToThread(worker_thread);
    qRegisterMetaType<SerialSetting>("SerialSetting");
    qRegisterMetaType<SerialSetting>("SerialSetting&");
    qRegisterMetaType<Status>("Status");
    qRegisterMetaType<Status>("Status&");
    qRegisterMetaType<Command>("Command");
    qRegisterMetaType<Command>("Command&");
    this->connect(this, &ScreenCheck::tell_worker_setting, this->worker, &Worker::accept_serial_setting,Qt::QueuedConnection);
    this->connect(this, &ScreenCheck::tell_worker_stm_command, this->worker, &Worker::accept_command_to_stm,Qt::QueuedConnection);
    this->connect(this, &ScreenCheck::tell_worker_stop_work, this->worker, &Worker::accept_stop_work,Qt::QueuedConnection);
    this->connect(this->worker, &Worker::tell_window_command,this, &ScreenCheck::accept_stm_command,Qt::QueuedConnection);
    this->connect(this->worker, &Worker::tell_window_stm_status,this, &ScreenCheck::accept_stm_status,Qt::QueuedConnection);
    this->connect(this->worker, &Worker::tell_window_work_step,this, &ScreenCheck::accept_worker_step,Qt::QueuedConnection);
    this->connect(this->worker, &Worker::tell_window_stm_respond_timeout,this, &ScreenCheck::accept_stm_respond_timeout,Qt::QueuedConnection);
    this->worker_thread->start();

    qpixmap = new QPixmap;
    pixitem = new PixItem(qpixmap);
    graph_scene = new QGraphicsScene;
    graph_view_preview = new QGraphicsView;
    graph_view_preview->setScene(graph_scene);
    graph_scene->addItem(pixitem);

    this->ui->pushButton_7->setChecked(false);
}

ScreenCheck::~ScreenCheck()
{
    delete qpixmap;
    delete qimage;
    delete ui;
    delete graph_view_preview;
    worker_thread->quit();
    worker_thread->wait();
    delete worker_thread;
    bool exist = QFile::exists("./database/hobject/good.bmp");
    if (exist)
        QFile::remove("./database/hobject/good.bmp");

}


void ScreenCheck::addlabel(const QString &name, const QString &content)
{
    this->object_label_map[name] = new Label(this);
    this->object_label_map[name]->setText(content);
}

void ScreenCheck::start_check()
{
   this->ui->stackedWidget->setCurrentIndex(1);
    //this->ui->label_8->hide();
}

int ScreenCheck::getMaxID()
{
    int id;
    QSqlDatabase *database = DB::getInterface();
    if (!database->open())
    {
        qDebug() << "Error: Failed to connect database." << database->lastError();
        return -1;
    }
    else
    {
        QSqlQuery query(*database);
        QString temp = QString("SELECT max(product_id) FROM checked_result;");
        if (!query.exec(temp))
        {
            qDebug() << "query error" << database->lastError();
            return -2;
        }
        while (query.next())
        {
            QSqlRecord rec = query.record();
            id = rec.value(0).toInt();
        }
        return id;
    }
}

void ScreenCheck::setMen(const Meninfo &info)
{
    this->men_info = info;
}


void ScreenCheck::on_pushButton_4_released()
{
    emit tell_window_step_page(1);
    emit tell_worker_stop_work();
}


void ScreenCheck::setSerial(QSerialPort *serial_)
{
    this->serial = serial_;
}

void ScreenCheck::setCamera(HKCamera *camera_)
{
    this->camera = camera_;
}


void ScreenCheck::serial_send_start()
{
     unsigned char send_buff[6];
     send_buff[0] = 0x55;
     send_buff[1] = 0x01;
     send_buff[2] = 0x01;
     send_buff[3] = 0x00;
     send_buff[4] = 0x41;
     send_buff[5] = 0xB8;
     serial->write((char *)send_buff,6);
}

void ScreenCheck::serial_send_end()
{
    unsigned char send_buff[6];//结束测试
    send_buff[0] = 0x55;
    send_buff[1] = 0xFF;
    send_buff[2] = 0x00;
    send_buff[3] = 0x00;
    send_buff[4] = 0x21;
    send_buff[5] = 0xD8;
    serial->write((char *)send_buff,6);
    id=1;
}

void ScreenCheck::serial_send_change_image()
{
    unsigned char send_buff[6];//切换图片
    unsigned short int crc_date;
    send_buff[0] = 0x55;
    send_buff[1] = 0x03;
    send_buff[2] = 0x01;
    send_buff[3] = id;
    crc_date=CRC16(send_buff,4);
    send_buff[4] = (unsigned char)(crc_date&0XFF);
    send_buff[5] = (unsigned char)(crc_date>>8);
    serial->write((char *)send_buff,6);
    id++;
}

void ScreenCheck::save_check_result(int product_id)
{
    QSqlDatabase *db = DB::getInterface();
    if (!db->open()){
        qDebug() << "Error: Failed to connect database." << db->lastError();
        return;
     }
    QSqlQuery query(*db);

    QString str = QString("INSERT INTO checked_result (product_id,check_result,wrap_result,scratch,white_point,black_point,time,men_id,men_name) VALUES (\"%1\", \"%2\", \"%3\", \"%4\", \"%5\", \"%6\", \"%7\", \"%8\", \"%9\" );")
            .arg(product_id)
            .arg(current_check_result.isQualified_s)
            .arg(current_check_result.wrapedOver_s)
            .arg(current_check_result.scratch)
            .arg(current_check_result.whitePoint)
            .arg(current_check_result.blackPoint)
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
            .arg(men_info.number)
            .arg(men_info.name);

    if (!query.exec(str)){
          qDebug() << "insert failed." << db->lastError();
          db->close();
          return;
    }
    else
        qDebug() << "insert successed";
    db->close();
}


QLabel *ScreenCheck::camera_label()
{
    return this->ui->preview;
}

void ScreenCheck::ImageCapture()
{
    if (MV_OK != camera->startCollect())
        return;
    if (MV_OK != camera->collectFrame(this->ui->preview))
        return;
    HObject image = camera->getImage();
    HObject deal_image;
    DefectsDetect detect;
    Hlong winid = (Hlong)this->ui->preview->winId();
    detect.run(image,deal_image,this->ui->preview->width(),this->ui->preview->height(),winid,this->ui->preview->x(),this->ui->preview->y());
    current_check_result.isQualified = detect.get_result();
    if (!current_check_result.isQualified){
        int itype =  detect.get_defectsType();
        current_check_result.isQualified_s = "bad";
        current_check_result.scratch = (itype & 0x01) == 0x01 ? "true" : "false";
        current_check_result.whitePoint = (itype & 0x02) == 0x02 ? "true" : "false";
        current_check_result.blackPoint = (itype & 0x04) == 0x04 ? "true" : "false";
    }
    else {
        current_check_result.isQualified_s = "good";
        current_check_result.scratch = "false";
        current_check_result.whitePoint = "false";
        current_check_result.blackPoint = "false";
    }
    if (current_check_result.isQualified){
        this->ui->label_5->setStyleSheet("border:1px solid black;background-color: rgb(255, 255, 255);image: url(:/image/合格.png);");
        WriteImage(deal_image, "bmp", 0, HTuple("./database/hobject/good"));
    }
    else{
        this->ui->label_5->setStyleSheet("border:1px solid black;background-color: rgb(255, 255, 255);image: url(:/image/\344\270\215\345\220\210\346\240\274.png);");
        this->filename = QDate::currentDate().toString("yyyyMMdd_") + QString("%1").arg(this->getMaxID()+1);
        HTuple hv_name1 = filename.toStdString().c_str();
        WriteImage(deal_image, "bmp", 0, HTuple("./database/hobject/") + hv_name1);
    }
    this->ui->stackedWidget->setCurrentIndex(1);
//    camera->HObjectToQImage(*himage,&qimage);
//    this->ui->preview->setPixmap(QPixmap::fromImage(*qimage));
}

bool ScreenCheck::workerSerialIsOpen()
{
    emit ask_worker_serial_status();
    msleep(1);
    return this->worker_thread_serial_status;
}

void ScreenCheck::accept_stm_command(Command com, int data)
{
    if (com == Command::ImageCapture)
    {
        this->ImageCapture();
        emit tell_worker_stm_command(Command::CheckResult,this->current_check_result.isQualified);
    }
    else if (com == Command::WrapResult)
    {
        this->current_check_result.wrapedOver = data;
        if (current_check_result.wrapedOver) this->current_check_result.wrapedOver_s = "good"; else this->current_check_result.wrapedOver_s = "bad";
        if (!data){
            this->ui->label_6->setStyleSheet("border:1px solid black;background-color: rgb(255, 255, 255);image: url(:/image/\344\270\215\345\220\210\346\240\274.png);");
        }
        else {
            this->ui->label_6->setStyleSheet("border:1px solid black;background-color: rgb(255, 255, 255);image: url(:/image/合格.png);");
        }
        this->save_check_result(this->getMaxID()+1);
    }
}

void ScreenCheck::accept_stm_respond_timeout()
{
    this->ui->label->setText("\344\270\213\344\275\215\346\234\272\345\223\215\345\272\224\350\266\205\346\227\266");
    QMessageBox messageBox;
    messageBox.setWindowTitle("警告");
    messageBox.setIcon(QMessageBox::Critical);
    messageBox.setText("\344\270\213\344\275\215\346\234\272\345\223\215\345\272\224\350\266\205\346\227\266，\345\267\262\347\273\217\345\201\234\346\255\242\345\267\245\344\275\234，\350\257\267\346\237\245\346\230\216\345\216\237\345\233\240\345\271\266\346\216\222\351\231\244\346\225\205\351\232\234\345\220\216\351\207\215\346\226\260\345\220\257\345\212\250");
    QPushButton button("确定");
    messageBox.addButton(&button, QMessageBox::YesRole);
    messageBox.exec();
}

void ScreenCheck::accept_stm_status(Status status){

    this->ui->Estop_s->setChecked(!status.E_Stop);
    this->ui->motor_1->setChecked(!status.transportMotorSpeedStatus);
    this->ui->motor_2->setChecked(!status.rollMontorSpeedStatus);
    this->ui->motor_3->setChecked(!status.rollerMotorSpeedStatus);
    this->ui->motor_4->setChecked(!status.slidingTableMotorSpeedStatus);

    this->ui->motor_1_speed->setValue(status.transport_motor_speed);
    this->ui->motor_2_speed->setValue(status.roll_motor_speed);
    this->ui->motor_3_speed->setValue(status.roller_motor_speed);
    this->ui->motor_4_speed->setValue(status.slidingtable_motor_speed);

    if(this->ui->Estop_s->isChecked()) this->ui->Estop_s->setText("\345\267\245\344\275\234\344\270\255"); else  this->ui->Estop_s->setText("\346\200\245\345\201\234\344\270\255");
    if(this->ui->motor_1->isChecked()) this->ui->motor_1->setText("正常"); else  this->ui->motor_1->setText("\346\225\205\351\232\234");
    if(this->ui->motor_2->isChecked()) this->ui->motor_2->setText("正常"); else  this->ui->motor_2->setText("\346\225\205\351\232\234");
    if(this->ui->motor_3->isChecked()) this->ui->motor_3->setText("正常"); else  this->ui->motor_3->setText("\346\225\205\351\232\234");
    if(this->ui->motor_4->isChecked()) this->ui->motor_4->setText("正常"); else  this->ui->motor_4->setText("\346\225\205\351\232\234");

    emit tell_window_stm_status(status);
}

void ScreenCheck::accept_worker_step(int step)
{
    if (step == 0){
        this->ui->label_5->setStyleSheet("border:1px solid black;background-color: rgb(255, 255, 255);");
        this->ui->label_6->setStyleSheet("border:1px solid black;background-color: rgb(255, 255, 255);");
        this->ui->pushButton_7->setChecked(true);
        this->ui->label->setText("\345\207\206\345\244\207\345\267\245\344\275\234");
        this->ui->label->adjustSize();
        this->ui->stackedWidget->setCurrentIndex(0);
    }
    else if (step == 1) {
        this->ui->pushButton_5->setChecked(true);
        this->ui->label->setText("\346\237\245\350\257\242\344\270\213\344\275\215\346\234\272\347\212\266\346\200\201");
        this->ui->label->adjustSize();
    }
    else if (step == 2) {
        this->ui->pushButton_2->setChecked(true);
        this->ui->label->setText("\345\274\200\345\247\213\345\267\245\344\275\234\345\271\266\347\255\211\345\276\205\345\233\276\345\203\217\351\207\207\351\233\206");
        this->ui->label->adjustSize();
    }

    else if (step == 3) {
        this->ui->pushButton->setChecked(true);
        this->ui->label->setText("\344\270\213\345\217\221\351\207\207\351\233\206\347\273\223\346\236\234");
        this->ui->label->adjustSize();
    }
    else if (step == 4) {
        this->ui->pushButton_3->setChecked(true);
        this->ui->label->setText("\344\270\213\345\217\221\345\256\214\346\210\220\345\271\266\347\255\211\345\276\205\345\214\205\350\243\205\347\273\223\346\236\234");
        this->ui->label->adjustSize();
    }
    else if (step == 5) {
        this->ui->pushButton_6->setChecked(true);
        this->ui->label->setText("\346\224\266\345\210\260\345\214\205\350\243\205\347\273\223\346\236\234");
        this->ui->label->adjustSize();
    }
}

void ScreenCheck::on_start_check_released()
{
    emit ask_serial_setting();
    emit tell_worker_setting(this->serial_setting);
}

//delete qpixmap;
//delete pixitem;
//qpixmap = new QPixmap("./database/hobject/" + this->filename + ".bmp");
//pixitem = new PixItem(qpixmap);

//graph_scene->clear();
//graph_scene->addItem(pixitem);
//graph_view_preview->resize(qpixmap->width(), qpixmap->height());
//graph_view_preview->show();

void ScreenCheck::on_pushButton_17_released()
{
    emit tell_worker_stm_command(Command::StopWork,0);
}

void ScreenCheck::on_pushButton_18_released()
{
    emit tell_worker_stm_command(Command::Reset,0);
}
