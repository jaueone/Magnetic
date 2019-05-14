#include "driveseting.h"
#include "ui_driveseting.h"
#include <QMap>
#include <QDebug>

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "None");

DriveSeting::DriveSeting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DriveSeting)
{
    ui->setupUi(this);
    serial = new QSerialPort(this);
    this->ui->pushButton_7->setText("打开串口");
    this->scan_serial();
    this->connect(this->ui->serialPortInfoListBox,&ComboBox::tell_serial_scan, this, &DriveSeting::accept_scan_serial);
}

DriveSeting::~DriveSeting()
{
    delete ui;
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

void DriveSeting::accept_scan_serial()
{
    this->scan_serial();
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


void DriveSeting::on_pushButton_7_released()
{

    if (this->serial->isOpen()){
        serial->close();
        this->ui->pushButton_7->setChecked(false);
        this->ui->pushButton_7->setText("打开串口");
    }

    else {
        SerialSetting setting = get_serial_setting();
        serial->setPortName(setting.name);
        serial->setBaudRate(setting.baudRate);
        serial->setDataBits(setting.dataBits);
        serial->setParity(setting.parity);
        serial->setStopBits(setting.stopBits);
        serial->setFlowControl(setting.flowControl);
        if (serial->open(QIODevice::ReadWrite) && serial->isOpen()){
            this->ui->pushButton_7->setChecked(true);
            this->ui->pushButton_7->setText("关闭串口");
        }
        else {
            this->ui->pushButton_7->setChecked(false);
            this->ui->pushButton_7->setText("打开串口");
            QMessageBox messageBox;
            messageBox.setWindowTitle("警告");
            messageBox.setIcon(QMessageBox::Warning);
            messageBox.setText("串口打开失败 ");
            QPushButton button("确定");
            messageBox.addButton(&button, QMessageBox::YesRole);
            messageBox.exec();
        }
    }
}

