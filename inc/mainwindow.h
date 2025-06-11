#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "SerialPort.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE


//void graph(nmea nmea, std::vector<double> horizontalPos);

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_start_clicked();

    void on_btnConnectReceiver_clicked();

    void on_btnDisconnectReceiver_clicked();

    void on_pathBtn_clicked();


    void on_btnRefreshPortList_clicked();

private:
    Ui::MainWindow *ui;
    SerialPort serialPort;
    void loadPorts();
    void readData(QByteArray data);
    void setupReceiver();
    QSerialPort::BaudRate baudRate();
    QSerialPort::DataBits dataBits();
    QSerialPort::Parity parity();
    QSerialPort::StopBits stopBits();
    QSerialPort::FlowControl flowControl();
};
#endif // MAINWINDOW_H
