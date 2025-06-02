#include "SerialPort.h"

SerialPort::SerialPort(QObject* parent) : QObject{parent}, serialPort(nullptr)
{
}


bool SerialPort::connect(QString portName, QSerialPort::BaudRate baudRate, QSerialPort::DataBits dataBits,
    QSerialPort::Parity parity, QSerialPort::StopBits stopBits ){
    qDebug() << "if open close";
    if(serialPort != nullptr){
        serialPort->close();
        delete serialPort;
    }
    qDebug() << "Setup port";
    serialPort = new QSerialPort(this);
    serialPort->setPortName(portName);
    serialPort->setBaudRate(baudRate);
    serialPort->setDataBits(dataBits);
    serialPort->setParity(parity);
    serialPort->setStopBits(stopBits);
    qDebug() << "connect port";
    if(serialPort->open(QIODevice::ReadWrite)){
        QObject::connect(serialPort, &QSerialPort::readyRead, this, &SerialPort::dataReady);
    }
    return serialPort->isOpen();
}

bool SerialPort::disconnect(){
    if(serialPort != nullptr){
        serialPort->close();
        serialPort = nullptr;
        return true;
    }
    return false;
}

void SerialPort::dataReady(){
    if(serialPort->isOpen()){
        emit dataReveived(serialPort->readLine());  
    }
}


