#include "serial.h"

bool Serial::open(const SerialSetting &setting, QIODevice::OpenMode model)
{
    if (__serial__->isOpen())
        __serial__->close();
    __serial__->setPortName(setting.name);
    __serial__->setBaudRate(setting.baudRate);
    __serial__->setDataBits(setting.dataBits);
    __serial__->setParity(setting.parity);
    __serial__->setStopBits(setting.stopBits);
    __serial__->setFlowControl(setting.flowControl);
    if (__serial__->open(model) && __serial__->isOpen()){
        return true;
    }
    else
        return false;
}

void Serial::scan_serial(QComboBox *serialPortInfoListBox)
{
    serialPortInfoListBox->clear();
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

        serialPortInfoListBox->addItem(list.first(), list);
        qDebug() << list;
    }
    serialPortInfoListBox->addItem("Custom");
}

QByteArray Serial::get_serial_bin(const SerialSetting &setting)
{
    QJsonObject serial
    {
        {"name",setting.name},
        {"baudRate",setting.stringBaudRate},
        {"dataBits",setting.stringDataBits},
        {"parity",setting.stringParity},
        {"stopBits",setting.stringStopBits},
        {"flowControl",setting.stringFlowControl},
    };
    QJsonDocument serial_doc = QJsonDocument(serial);
    QByteArray serial_data = serial_doc.toJson();
    return serial_data;
}

Serial::Serial()
{

}

