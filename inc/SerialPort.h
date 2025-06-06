#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>

struct ReceiverInfo{
    QString portName;
    QSerialPort::BaudRate baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::Parity parity;
    QSerialPort::StopBits stopBits;
    QSerialPort::FlowControl flowControl;
};

class SerialPort : public QObject
{
    Q_OBJECT
public:
    explicit SerialPort(QObject *parent = nullptr);
    void setupReceiver(QString portName,QSerialPort::BaudRate baudRate, QSerialPort::DataBits dataBits,
    QSerialPort::Parity parity, QSerialPort::StopBits stopBits, QSerialPort::FlowControl flowControl);
    bool connect();
    bool disconnect();
signals:
    void dataReceived(QByteArray data);
private slots:
    void dataReady();
private:
    QSerialPort* serialPort;
    ReceiverInfo receiverInfo;
};
#endif
