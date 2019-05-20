#ifndef SERIAL_H
#define SERIAL_H
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QComboBox>
#include <QDebug>

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "None");

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

class Serial
{
public:
    Serial();
    static void scan_serial(QComboBox * serialPortInfoListBox){
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

    static QByteArray get_serial_bin(const SerialSetting &setting){
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
};

#endif // SERIAL_H
