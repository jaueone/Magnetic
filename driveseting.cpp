#include "driveseting.h"
#include "ui_driveseting.h"
#include "my_control.h"
#include "DefectsDetect.h"
#include <QMap>
#include <QString>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSerialPort>




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

unsigned short int DriveSeting::CRC16(unsigned char *pchMsg, unsigned short int wDataLen)
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

DriveSeting::DriveSeting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DriveSeting)
{
    ui->setupUi(this);
    serial = new QSerialPort(this);
    camera = HKCamera::getInterface();
    this->ui->pushButton_7->setText(QString::fromLocal8Bit("打开串口"));
    this->ui->baudRateBox->setCurrentIndex(1);
    this->ui->dataBitsBox->setCurrentIndex(3);

    QWidget *widgets = new QWidget;
    this->ui->tabWidget->setCornerWidget(widgets,Qt::TopRightCorner);
    QWidget *widget = this->ui->tabWidget->cornerWidget();
    widget->setStyleSheet("background-color:rgb(255,0,0);");
    qDebug() << widget->size();
    this->connect(this->ui->serialPortInfoListBox,&ComboBox::clicked, this, &DriveSeting::accept_scan_serial);

    this->scan_serial();
    this->load_setting();
    this->load_algorithm();
}

DriveSeting::~DriveSeting()
{
    delete ui;
}

bool DriveSeting::init()
{
    this->scan_serial();
    this->load_setting();

    if (this->init_camera() != "opened")
        this->Camera_OK = false;
    else
        this->Camera_OK = true;
    if (this->init_serial() != "opened")
        this->Serial_OK = false;
    else
        this->Serial_OK = true;

    if (Camera_OK && Serial_OK)
    {
        serial->close();
        return true;
    }
    else
    {
        serial->close();
        return false;
    }
}

void DriveSeting::display_init()
{
    QMessageBox messageBox;
    messageBox.setWindowTitle(QString::fromLocal8Bit("警告"));
    messageBox.setIcon(QMessageBox::Warning);
    QPushButton button(QString::fromLocal8Bit("确定"));
    messageBox.addButton(&button, QMessageBox::YesRole);
    if (!this->Camera_OK && !this->Serial_OK){
        messageBox.setText(QString::fromLocal8Bit("相机自检失败  \n串口自检失败   请联系管理员"));
        messageBox.exec();
    }
    else if (!this->Serial_OK){
        messageBox.setText(QString::fromLocal8Bit("串口自检失败   请联系管理员"));
        messageBox.exec();
    }
    else if (!this->Camera_OK){
        messageBox.setText(QString::fromLocal8Bit("相机自检失败   请联系管理员"));
        messageBox.exec();
    }
    else{
        messageBox.setWindowTitle("信息");
        messageBox.setIcon(QMessageBox::Information);
        messageBox.setText(QString::fromLocal8Bit("系统自检成功"));
        messageBox.exec();
    }
}

QString DriveSeting::init_camera()
{
    if (MV_OK != camera->enumDevices())
        return "enum fail";
    if (MV_OK != camera->openDevice(0))
        return  "open fail";
    this->set_camera_params();
    return "opened";
}

QString DriveSeting::init_serial()
{
    if (serial->isOpen())
        serial->close();
    SerialSetting setting = get_serial_setting();
    serial->setPortName(setting.name);
    serial->setBaudRate(setting.baudRate);
    serial->setDataBits(setting.dataBits);
    serial->setParity(setting.parity);
    serial->setStopBits(setting.stopBits);
    serial->setFlowControl(setting.flowControl);
    if (serial->open(QIODevice::ReadWrite) && serial->isOpen()){

        return "opened";
    }
    return "opened fail";
}

SerialSetting DriveSeting::get_serial_setting()
{
    QMap<QString, int> map = {{"4800",QSerialPort::Baud4800},
                        {"9600",QSerialPort::Baud9600},
                        {"19200",QSerialPort::Baud19200},
                        {"38400",QSerialPort::Baud38400},
                        {"115200",QSerialPort::Baud115200},
                        {"5", QSerialPort::Data5},
                        {"6", QSerialPort::Data6},
                        {"7", QSerialPort::Data7},
                        {"8", QSerialPort::Data8},
                        {"None", QSerialPort::NoParity},
                        {"Even", QSerialPort::EvenParity},
                        {"Odd", QSerialPort::OddParity},
                        {"Mark", QSerialPort::MarkParity},
                        {"Space", QSerialPort::SpaceParity},
                        {"1", QSerialPort::OneStop},
                        {"1.5", QSerialPort::OneAndHalfStop},
                        {"2", QSerialPort::TwoStop},
                        {"None", QSerialPort::NoFlowControl},
                        {"RTS/CTS", QSerialPort::HardwareControl},
                        {"XON/XOFF", QSerialPort::SoftwareControl},
                       };

    SerialSetting setting;
    setting.name = this->ui->serialPortInfoListBox->currentText();

    setting.baudRate = static_cast<QSerialPort::BaudRate>(map[this->ui->baudRateBox->currentText()]);
    setting.stringBaudRate = QString::number(setting.baudRate);

    setting.dataBits = static_cast<QSerialPort::DataBits>(map[this->ui->dataBitsBox->currentText()]);
    setting.stringDataBits = this->ui->dataBitsBox->currentText();

    setting.parity = static_cast<QSerialPort::Parity>(map[this->ui->parityBox->currentText()]);
    setting.stringParity = this->ui->parityBox->currentText();

    setting.stopBits = static_cast<QSerialPort::StopBits>(map[this->ui->stopBitsBox->currentText()]);
    setting.stringStopBits = this->ui->stopBitsBox->currentText();

    setting.flowControl = static_cast<QSerialPort::FlowControl>(map[this->ui->flowControlBox->currentText()]);
    setting.stringFlowControl = this->ui->flowControlBox->currentText();

    return setting;
}

CameraSetting DriveSeting::get_camera_setting()
{
    QMap<QString, int> map = {
        {"Off"  , 0},
        {"Once" , 1},
        {"Continuous", 2},  //GainAuto
        {"User", 1},
        {"sRGB", 2},    //GammaSelector
        {"Line0", 0},
        {"Line1", 1},
        {"Line2", 2},
        {"Line3", 3},
        {"Counter0", 4},
        {"Software", 7},
        {"FrequencyConverter", 8},  //TriggerSource
        {"FrameBurstStart", 6},
        {"LineStart", 9},       //TriggerSelector
    };
    QMap<QString, int> map_1{
        {"Off", 0},
        {"On", 1},      //TriggerMode
        {"Line0", 0},
        {"Line1", 1},
        {"Line2", 2},
        {"Line3", 3},
        {"Line4", 4},   //LineSelector
    };
    CameraSetting setting;
    setting.width = static_cast<unsigned int>(this->ui->spinBox_3->value());
    setting.height = static_cast<unsigned int>(this->ui->spinBox_4->value());
    setting.offsetX = static_cast<unsigned int>(this->ui->spinBox_5->value());
    setting.offsetY = static_cast<unsigned int>(this->ui->spinBox_6->value());
    setting.acquisitionLineRate = static_cast<unsigned int>(this->ui->spinBox_7->value());
    setting.acquisitionLineRateEnable = this->ui->checkBox->isChecked();
    setting.exposureTime = static_cast<float>(this->ui->doubleSpinBox->value());

    setting.gain = static_cast<float>(this->ui->doubleSpinBox_2->value());
    setting.gainAuto =static_cast<GainAuto>(map[this->ui->comboBox->currentText()]);

    setting.gamma = static_cast<float>(this->ui->doubleSpinBox_3->value());
    setting.gammaSelector = static_cast<GammaSelector>(map[this->ui->comboBox_2->currentText()]);
    setting.gammaEnable = this->ui->checkBox_2->isChecked();
    setting.nucEnable = this->ui->checkBox_3->isChecked();
    setting.triggerSelector = static_cast<TriggerSelector>(map[this->ui->comboBox_3->currentText()]);
    setting.triggerMode = static_cast<TriggerMode>(map_1[this->ui->comboBox_4->currentText()]);
    setting.triggerSource = static_cast<TriggerSource>(map[this->ui->comboBox_5->currentText()]);
    setting.lineSelector = static_cast<LineSelector>(map_1[this->ui->comboBox_6->currentText()]);
    setting.preDivider = static_cast<unsigned int>(this->ui->spinBox_8->value());
    setting.multiplier = static_cast<unsigned int>(this->ui->spinBox_9->value());
    setting.gevSCPSPacketSize = static_cast<unsigned int>(this->ui->spinBox_10->value());
    return setting;
}


void DriveSeting::load_setting()
{
    QSqlDatabase *database = DB::getInterface();
    QJsonDocument ser_doc;
    QJsonDocument cam_doc;
    if (!database->open())
    {
       qDebug() << "Error: Failed to connect database." << database->lastError();
       return;
    }
    else {
        QSqlQuery query(*database);
        QString temp = QString("SELECT serial_config,camera_config FROM admin_config WHERE id = 1;");
        if (!query.exec(temp))
            return;

        while (query.next()) {
            QSqlRecord rec = query.record();
            ser_doc = QJsonDocument::fromJson(rec.value("serial_config").toByteArray());
            cam_doc = QJsonDocument::fromJson(rec.value("camera_config").toByteArray());
        }
        database->close();
    }

    QJsonObject ser_obj = ser_doc.object();
    QJsonObject cam_obj = cam_doc.object();

    this->ui->spinBox_3->setValue(cam_obj["width"].toInt());
    this->ui->spinBox_4->setValue(cam_obj["height"].toInt());
    this->ui->spinBox_5->setValue(cam_obj["offsetX"].toInt());
    this->ui->spinBox_6->setValue(cam_obj["offsetY"].toInt());
    this->ui->spinBox_7->setValue(cam_obj["acquisitionLineRate"].toInt());

    this->ui->checkBox->setChecked(cam_obj["acquisitionLineRateEnable"].toBool());

    this->ui->doubleSpinBox->setValue(cam_obj["exposureTime"].toDouble());
    this->ui->doubleSpinBox_2->setValue(cam_obj["gain"].toDouble());

    this->ui->comboBox->setCurrentText(cam_obj["gainAuto"].toString());
    this->ui->doubleSpinBox_3->setValue(cam_obj["gamma"].toDouble());

    this->ui->comboBox_2->setCurrentText(cam_obj["gammaSelector"].toString());
    this->ui->checkBox_2->setChecked(cam_obj["gammaEnable"].toBool());
    this->ui->checkBox_3->setChecked(cam_obj["nucEnable"].toBool());

    this->ui->comboBox_3->setCurrentText(cam_obj["triggerSelector"].toString());
    this->ui->comboBox_4->setCurrentText(cam_obj["triggerMode"].toString());
    this->ui->comboBox_5->setCurrentText(cam_obj["triggerSource"].toString());
    this->ui->comboBox_6->setCurrentText(cam_obj["lineSelector"].toString());

    this->ui->baudRateBox->setCurrentText(ser_obj["baudRate"].toString());
    this->ui->dataBitsBox->setCurrentText(ser_obj["dataBits"].toString());
    this->ui->parityBox->setCurrentText(ser_obj["parity"].toString());
    this->ui->stopBitsBox->setCurrentText(ser_obj["stopBits"].toString());
    this->ui->flowControlBox->setCurrentText(ser_obj["flowControl"].toString());

    this->ui->spinBox_8->setValue(cam_obj["preDivider"].toInt());
    this->ui->spinBox_9->setValue(cam_obj["multiplier"].toInt());
    this->ui->spinBox_10->setValue(cam_obj["gevSCPSPacketSize"].toInt());

    this->ui->serialPortInfoListBox->setCurrentText(ser_obj["name"].toString());
}

void DriveSeting::load_algorithm()
{
    QJsonObject params;
    QSqlDatabase *db = DB::getInterface();
    QSqlQuery query(*db);
    if (!db->open()){
        qDebug() << "open database failed";
        return;
    }
    QString temp = QString("SELECT data FROM algorithm WHERE type = \"%1\";").arg("algorithmic_parameters");
    if (!query.exec(temp))
    {
        qDebug() << "exec failed";
        return;
    }
    else
    {
        qDebug() << "exec successed";
    }
    query.next();
    QSqlRecord rec = query.record();
    QByteArray JSON_S = rec.value(0).toByteArray();
    params = QJsonDocument::fromJson(JSON_S).object();
    if (params.isEmpty()){
        return;
    }
    QList<QDoubleSpinBox*> list = this->ui->groupBox_5->findChildren<QDoubleSpinBox*>();
    QList<QDoubleSpinBox*> list1 = this->ui->groupBox_6->findChildren<QDoubleSpinBox*>();
    QList<QDoubleSpinBox*> list2 = this->ui->groupBox_7->findChildren<QDoubleSpinBox*>();
    for (auto object:list) {
        if(!params[object->objectName()].toDouble())
            continue;
        object->setValue(params[object->objectName()].toDouble());
    }
    for (auto object:list1) {
        if(!params[object->objectName()].toDouble())
            continue;
        object->setValue(params[object->objectName()].toDouble());
    }
    for (auto object:list2) {
        if(!params[object->objectName()].toDouble())
            continue;
        object->setValue(params[object->objectName()].toDouble());
    }
}

QJsonObject DriveSeting::get_algorithm()
{
    QJsonObject params;
    QList<QDoubleSpinBox*> list = this->ui->groupBox_5->findChildren<QDoubleSpinBox*>();
    QList<QDoubleSpinBox*> list1 = this->ui->groupBox_6->findChildren<QDoubleSpinBox*>();
    QList<QDoubleSpinBox*> list2 = this->ui->groupBox_7->findChildren<QDoubleSpinBox*>();
    for (auto object:list) {
        params[object->objectName()] = object->value();
    }
    for (auto object:list1) {
        params[object->objectName()] = object->value();
    }
    for (auto object:list2) {
        params[object->objectName()] = object->value();
    }
    return params;
}

void DriveSeting::save_setting(SerialSetting setting_ser, CameraSetting setting_cam)
{
    QByteArray serial = Serial::get_serial_bin(setting_ser);
    QByteArray camera = HKCamera::get_camera_bin(setting_cam);
    qDebug() << camera;

    QString str_serial = serial;
    QString str_camera = camera;
    QSqlDatabase *database = DB::getInterface();

    if (!database->open())
    {
        qDebug() << "Error: Failed to connect database." << database->lastError();
    }

    else {
         QSqlQuery query(*database);
         QString temp = QString("UPDATE admin_config SET serial_config = :serial , camera_config = :camera WHERE id = 1;");
         query.prepare(temp);
         query.bindValue(":serial", serial);
         query.bindValue(":camera", camera);
         if (query.exec()){
             QSqlRecord rec = query.record();
             qDebug() << "saved";
         }
         database->close();
    }
}

void DriveSeting::scan_serial()
{
    this->ui->serialPortInfoListBox->clear();
    QString description;
    QString manufacturer;
    QString serialNumber;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);

        this->ui->serialPortInfoListBox->addItem(list.first(), list);
        qDebug() << list;
    }
    this->ui->serialPortInfoListBox->addItem(tr("Custom"));
}

void DriveSeting::check_self()
{
    this->init();
    this->display_init();
}

void DriveSeting::set_camera_params()
{
    CameraSetting setting = this->get_camera_setting();

    qDebug("%x",this->camera->setParams(DType::Int, "Width", setting.width));
    qDebug("%x",this->camera->setParams(DType::Int, "Height", setting.height));
    qDebug("%x",this->camera->setParams(DType::Int, "OffsetX", setting.offsetX));
    qDebug("%x",this->camera->setParams(DType::Int, "OffsetY", setting.offsetY));
    qDebug("%x",this->camera->setParams(DType::Int, "AcquisitionLineRate", setting.acquisitionLineRate));
    qDebug("%x",this->camera->setParams(DType::Bool, "AcquisitionLineRateEnable", setting.acquisitionLineRateEnable));
    qDebug("%x",this->camera->setParams(DType::Float, "Gain", setting.gain));
    qDebug("%x",this->camera->setParams(DType::Enum, "GainAuto", setting.gainAuto));
    qDebug("%x",this->camera->setParams(DType::Float, "Gamma", setting.gamma));
    qDebug("%x",this->camera->setParams(DType::Bool, "GammaEnable", setting.gammaEnable));
    qDebug("%x",this->camera->setParams(DType::Enum, "GammaSelector", setting.gammaSelector));
    qDebug("%x",this->camera->setParams(DType::Float, "ExposureTime", setting.exposureTime));
    qDebug("%x",this->camera->setParams(DType::Bool, "NUCEnable", setting.nucEnable));
    qDebug("%x",this->camera->setParams(DType::Enum, "TriggerMode", setting.triggerMode));
    qDebug("%x",this->camera->setParams(DType::Enum, "TriggerSelector", setting.triggerSelector));
    qDebug("%x",this->camera->setParams(DType::Enum, "TriggerSource", setting.triggerSource));
    qDebug("%x",this->camera->setParams(DType::Enum, "LineSelector", setting.lineSelector));
    qDebug("%x",this->camera->setParams(DType::Int, "PreDivider", setting.preDivider));
    qDebug("%x",this->camera->setParams(DType::Int, "Multiplier", setting.multiplier));
    qDebug("%x",this->camera->setParams(DType::Int, "GevSCPD", setting.gevSCPSPacketSize));
    this->ui->label_27->setText(QString::fromLocal8Bit("设置相机参数完成"));
}

void DriveSeting::accept_scan_serial()
{
    this->scan_serial();
}

void DriveSeting::accept_stm_status(const Status &status)
{
    this->ui->motor_1_speed->setValue(status.transport_motor_speed);
    this->ui->motor_2_speed->setValue(status.roll_motor_speed);
    this->ui->motor_3_speed->setValue(status.roller_motor_speed);
    this->ui->motor_4_speed->setValue(status.slidingtable_motor_speed);
}

void DriveSeting::accept_stm_respond_timeout()
{
    QMessageBox messageBox;
    messageBox.setWindowTitle(QString::fromLocal8Bit("警告"));
    messageBox.setIcon(QMessageBox::Critical);
    messageBox.setText(QString::fromLocal8Bit("下位机响应超时,已停止工作\n 需要查明故障修复后，"));
    QPushButton button(QString::fromLocal8Bit("确定"));
    messageBox.addButton(&button, QMessageBox::YesRole);
    messageBox.exec();
}

void DriveSeting::accept_serial_status(bool isopened)
{
    this->worker_serial_status = isopened;
    if (isopened){
        this->ui->pushButton_7->setChecked(true);
        this->ui->pushButton_7->setText(QString::fromLocal8Bit("关闭串口"));
    }
    else {
        this->ui->pushButton_7->setChecked(false);
        this->ui->pushButton_7->setText(QString::fromLocal8Bit("打开串口"));
    }
}

void DriveSeting::accept_stm_command(Command com, int data)
{
    if (com == Command::RespondOK && data == -3)
    {
        QMessageBox messageBox;
        messageBox.setWindowTitle(QString::fromLocal8Bit("信息"));
        messageBox.setIcon(QMessageBox::Information);
        messageBox.setText(QString::fromLocal8Bit("设置速度完成 "));
        QPushButton button(QString::fromLocal8Bit("确定"));
        messageBox.addButton(&button, QMessageBox::YesRole);
        messageBox.exec();
    }
}


void DriveSeting::on_serialPortInfoListBox_currentIndexChanged(int index)
{
    if (index == -1)
        return;
    const bool isCustomPath = !this->ui->serialPortInfoListBox->itemData(index).isValid();
    this->ui->serialPortInfoListBox->setEditable(isCustomPath);
    if (isCustomPath)
        this->ui->serialPortInfoListBox->clearEditText();
    const QStringList list = this->ui->serialPortInfoListBox->itemData(index).toStringList();
    this->ui->descriptionLabel->setText(tr("Description: %1").arg(list.count() > 1 ? list.at(1) : tr(blankString)));
    this->ui->manufacturerLabel->setText(tr("Manufacturer: %1").arg(list.count() > 2 ? list.at(2) : tr(blankString)));
    this->ui->serialNumberLabel->setText(tr("Serial number: %1").arg(list.count() > 3 ? list.at(3) : tr(blankString)));
    this->ui->locationLabel->setText(tr("Location: %1").arg(list.count() > 4 ? list.at(4) : tr(blankString)));
    this->ui->vidLabel->setText(tr("Vendor Identifier: %1").arg(list.count() > 5 ? list.at(5) : tr(blankString)));
    this->ui->pidLabel->setText(tr("Product Identifier: %1").arg(list.count() > 6 ? list.at(6) : tr(blankString)));
}

void DriveSeting::on_pushButton_released()
{
    emit tell_window_step_page(1);
}


void DriveSeting::on_pushButton_2_released()
{
    CameraSetting setting = this->get_camera_setting();
    this->save_setting(this->get_serial_setting(), setting);


    QMessageBox messageBox;
    messageBox.setWindowTitle(QString::fromLocal8Bit("信息"));
    messageBox.setIcon(QMessageBox::Information);
    messageBox.setText(QString::fromLocal8Bit("参数设置保存成功 "));
    QPushButton button(QString::fromLocal8Bit("确定"));
    messageBox.addButton(&button, QMessageBox::YesRole);
    messageBox.exec();
}


void DriveSeting::on_pushButton_4_released()
{
    this->get_camera_setting();
    qDebug() << "apply";
}

void DriveSeting::on_pushButton_7_clicked(bool checked)
{
    if (checked){
        emit tell_worker_open_serial(this->get_serial_setting());
        Sleep::sleep(1);
        if (!this->worker_serial_status){
            QMessageBox messageBox;
            messageBox.setWindowTitle(QString::fromLocal8Bit("警告"));
            messageBox.setIcon(QMessageBox::Warning);
            messageBox.setText(QString::fromLocal8Bit("串口打开失败,请先检查串口 "));
            QPushButton button(QString::fromLocal8Bit("确定"));
            messageBox.addButton(&button, QMessageBox::YesRole);
            messageBox.exec();
        }
    }
    else {
        emit tell_worker_stop_work();
    }
}

void DriveSeting::on_pushButton_9_released()
{
    if (MV_OK != camera->enumDevices()){
        QMessageBox messageBox;
        messageBox.setWindowTitle(QString::fromLocal8Bit("警告"));
        messageBox.setIcon(QMessageBox::Warning);
        messageBox.setText(QString::fromLocal8Bit("无法找到相机"));
        QPushButton button(QString::fromLocal8Bit("确定"));
        messageBox.addButton(&button, QMessageBox::YesRole);
        messageBox.exec();
        return;
    }
    if (MV_OK != camera->openDevice(0)){
        QMessageBox messageBox;
        messageBox.setWindowTitle(QString::fromLocal8Bit("警告"));
        messageBox.setIcon(QMessageBox::Warning);
        messageBox.setText(QString::fromLocal8Bit("相机打开失败"));
        QPushButton button(QString::fromLocal8Bit("确定"));
        messageBox.addButton(&button, QMessageBox::YesRole);
        messageBox.exec();
        return;
    }
//    this->set_camera_params();
    this->ui->label_27->setText(QString::fromLocal8Bit("打开相机完成"));
}

void DriveSeting::on_pushButton_11_clicked()
{
    if (MV_OK != camera->collectFrame(this->ui->label_18))
        return;
    this->ui->label_27->setText(QString::fromLocal8Bit("抓取一帧图片完成"));
}

void DriveSeting::on_pushButton_10_clicked()
{
    QSqlDatabase *database = DB::getInterface();
    QJsonDocument cam_doc;
    if (!database->open())
    {
       qDebug() << "Error: Failed to connect database." << database->lastError();
       return;
    }
    else {
        QSqlQuery query(*database);
        QString temp = QString("SELECT camera_config FROM admin_config WHERE id = 1;");
        if (!query.exec(temp))
            return;
        while (query.next()) {
            QSqlRecord rec = query.record();
            cam_doc = QJsonDocument::fromJson(rec.value("camera_config").toByteArray());
        }
        database->close();
    }
    QJsonObject cam_obj = cam_doc.object();
    CameraSetting setting = camera->get_camera_setting();
    this->save_setting(this->get_serial_setting(), setting);
    this->load_setting();
    QByteArray by = HKCamera::get_camera_bin(setting);
    QJsonObject j =  QJsonDocument::fromJson(by).object();

}


void DriveSeting::on_pushButton_12_clicked()
{
    if (!camera->isOpened()){
        HKCamera::camera_message_warn();
        return;
    }
    if (camera->isCollecting()){
        QMessageBox messageBox;
        messageBox.setWindowTitle(QString::fromLocal8Bit("警告"));
        messageBox.setIcon(QMessageBox::Warning);
        QPushButton button(QString::fromLocal8Bit("确定"));
        messageBox.setText(QString::fromLocal8Bit("先停止抓图在设置参数"));
        messageBox.exec();
        return;
    }
    this->set_camera_params();
    if(!camera->isOpened())
        return;
    HKCamera::camera_message_done();
}


void DriveSeting::on_pushButton_15_released()
{
    uint16_t transport_motor_speed = (uint16_t)this->ui->motor_1_speed->value();
    emit tell_worker_stm_command(Command::SetTransportMotorSpeed,transport_motor_speed);
}

void DriveSeting::on_pushButton_16_released()
{
    uint16_t roll_motor_speed = (uint16_t)this->ui->motor_1_speed->value();
    emit tell_worker_stm_command(Command::SetRollMontorSpeed,roll_motor_speed);
}

void DriveSeting::on_pushButton_19_released()
{
    uint16_t roller_motor_speed = (uint16_t)this->ui->motor_1_speed->value();
    emit tell_worker_stm_command(Command::SetRollerMotorSpeed,roller_motor_speed);
}

void DriveSeting::on_pushButton_20_released()
{
    uint16_t slidingtable_motor_speed = (uint16_t)this->ui->motor_1_speed->value();
    emit tell_worker_stm_command(Command::SetSlidingTableMotorSpeed,slidingtable_motor_speed);
}

void DriveSeting::on_pushButton_13_released()
{
    QString save_name;
    if (MV_OK != camera->startCollect())
        return;
    if (MV_OK != camera->collectFrame(this->ui->label_18))
        return;
    HObject image = camera->getImage();
    save_name= QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    HTuple hv_name1 = save_name.toStdString().c_str();
    WriteImage(image, "bmp", 0, HTuple("./photo/") + hv_name1);
}

void DriveSeting::on_pushButton_14_released()
{
    if (!camera->isOpened())
        return;
    if (MV_OK != camera->openDevice(0))
        return;
    if (MV_OK != camera->startCollect())
        return;
    if (MV_OK != camera->stopCollect())
        return;
    this->ui->label_27->setText(QString::fromLocal8Bit("停止抓图"));
}

void DriveSeting::on_pushButton_17_released()
{
    if (!camera->isOpened())
        return;
    if (MV_OK != camera->startCollect())
        return;
    if (MV_OK != camera->stopCollect())
        return;
    if (MV_OK != camera->closeDevice())
        return;
    this->ui->label_18->clear();
    this->ui->label_27->setText(QString::fromLocal8Bit("关闭相机"));
}

void DriveSeting::on_pushButton_18_released()
{
    if (!camera->isOpened())
        return;
    if (MV_OK != camera->startCollect())
        return;
    this->ui->label_27->setText(QString::fromLocal8Bit("开始抓图"));
}

void DriveSeting::on_pushButton_5_released()
{
    QJsonObject params;
    QSqlDatabase *db = DB::getInterface();
    if (!db->open()){
        qDebug() << "open database failed";
        return;
    }
    QList<QDoubleSpinBox*> list = this->ui->groupBox_5->findChildren<QDoubleSpinBox*>();
    QList<QDoubleSpinBox*> list1 = this->ui->groupBox_6->findChildren<QDoubleSpinBox*>();
    QList<QDoubleSpinBox*> list2 = this->ui->groupBox_7->findChildren<QDoubleSpinBox*>();
    for (auto object:list) {
        params[object->objectName()] = object->value();
    }
    for (auto object:list1) {
        params[object->objectName()] = object->value();
    }
    for (auto object:list2) {
        params[object->objectName()] = object->value();
    }
    QSqlQuery query(*db);
    QJsonDocument doc(params);
    QString temp = QString::fromLocal8Bit("UPDATE algorithm SET data = :data, time = :time WHERE type = :type;");
    query.prepare(temp);
    query.bindValue(":data",doc.toJson());
    query.bindValue(":time",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":type","algorithmic_parameters");

    if (!query.exec())
    {
        qDebug() << "exec failed";
    }
    else
    {
        qDebug() << "exec successed";
    }

    db->close();
}

void DriveSeting::on_pushButton_6_released()
{
    this->camera->collectFrame_2();
}
