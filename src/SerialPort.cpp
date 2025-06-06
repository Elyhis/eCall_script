#include "SerialPort.h"

SerialPort::SerialPort(QObject* parent) : QObject{parent}, serialPort(nullptr), receiverInfo(nullptr)
{
}

void SerialPort::setupReceiver(QString portName,QSerialPort::BaudRate baudRate, QSerialPort::DataBits dataBits,
    QSerialPort::Parity parity, QSerialPort::StopBits stopBits){
        receiverInfo.portName = portName;
        receiverInfo.baudRate = baudRate;
        receiverInfo.dataBits = dataBits;
        receiverInfo.parity = parity;
        receiverInfo.stopBits = stopBits;
    }

bool SerialPort::connect(){
    if(serialPort != nullptr){
        serialPort->close();
        delete serialPort;
    }
    serialPort = new QSerialPort(this);
    serialPort->setPortName(receiverInfo.portName);
    serialPort->setBaudRate(receiverInfo.baudRate);
    serialPort->setDataBits(receiverInfo.dataBits);
    serialPort->setParity(receiverInfo.parity);
    serialPort->setStopBits(receiverInfo.stopBits);

    if (!serialPort->open(QIODevice::ReadOnly)){
        return false;
    }
    QObject::connect(serialPort, &QSerialPort::readyRead, this, &SerialPort::dataReady);
    return true;
}

bool SerialPort::disconnect(){
    if(serialPort != nullptr){
        serialPort->close();
        serialPort = nullptr;
        return true;
    }
    return false;
}

void SerialPort::dataReady() {
    if (serialPort->isOpen()) {
        QByteArray line = serialPort->readLine();
        emit dataReceived(line);
    }
}