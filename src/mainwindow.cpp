#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QTextStream>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QFileDialog>


#include <tuple>
#include <vector>
#include <filesystem>
#include <iostream>
#include <fstream>

#include "lla.h"
#include "command_exception.h"
#include "date_time.h"


#include "nmea.h"

#include "Scenario.h"


using namespace Sdx;


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->cmbBaudRate->setCurrentIndex(3);
    loadPorts();
}

void MainWindow::loadPorts()
{
    ui->cmbReceiver->clear();
    foreach(auto &port, QSerialPortInfo::availablePorts()){
        ui->cmbReceiver->addItem(port.portName());
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_start_clicked()
{
    int nbIteration = 1;
    Scenario scenario;

    // If you wish to connect to the simulator running on a remote computer,
    // change "localhost" for the remote computer's IP address, such as "192.168.1.100"s
    const std::string HOST = "localhost";

    //Device type parameters
    const std::string TARGET_TYPE = "None"; // Change to "DTA-2116" to execute on a DTA-2116 device
    const std::string DEVICE_IP = "";           // Change to "192.168.XXX.XXX" to execute on a TARGET_TYPE device
    int duration = 60; // Time in seconde

    setupReceiver();
    
    //Path to nmea output data
    std::filesystem::path filePath = ui->path->text().toStdString();
    filePath = filePath / "eCall";

    //FIXME: Test purpose
    //filePath = "C:/Users/Bryan.Barbe/Downloads/250514_090632_2.2.2_gps_gal_sbas_rcv.nmea";

    auto now = std::chrono::system_clock::now();
    // Convert to local time
    auto localTime = std::chrono::system_clock::to_time_t(now);
    // Format the timestamp as a string
    std::stringstream ss;
    ss << std::put_time(std::localtime(&localTime), "%F_%H-%M-%S");

    std::filesystem::path reportPath = "report_" + ss.str() + ".html";

    ss.str("");

    ss << std::put_time(std::localtime(&localTime), "%d/%m/%Y %T");
    std::fstream report;
    report.open(reportPath, std::ios::out);
    report << "<body bgcolor=\"white\">";
    report << "<font face=\"Microsoft Sans Serif\" size=\"2\">";
    
    report << "<h4>eCall testing</h4>";
    report << "<div>Tests started " + ss.str() + "</div>";
    report << "<hr>";


    // std::cout << "==> Connecting to the receiver" << std::endl;
    // sim.call(ConnectSerialPortReceiver::create(SERIAL_PORT,BAUD_RATE,DATA_BITS,PARITY,STOP_BITS,FLOW_CONTROL));


    if(ui->staticGPS_Gal_SBAB->isChecked()){
        try{
            //graph(nmea, horizontalPos);
            scenario.eCallStatic(filePath, HOST, TARGET_TYPE, DEVICE_IP, duration, true, report);
            
        }catch (CommandException& e)
        {
        std::cout << "Simulator Command Exception caught:\n" << e.what() << std::endl;
        }
        catch (std::runtime_error& e)
        {
            std::cout << "Runtime Error Exception caught:\n" << e.what() << std::endl;
        }
        report << "<hr>";
    }
    if(ui->staticGal->isChecked()){
        try{
            scenario.eCallStaticGal(filePath, HOST, TARGET_TYPE, DEVICE_IP, duration, report);
        }catch (CommandException& e)
        {
        std::cout << "Simulator Command Exception caught:\n" << e.what() << std::endl;
        }
        catch (std::runtime_error& e)
        {
            std::cout << "Runtime Error Exception caught:\n" << e.what() << std::endl;
        }
        report << "<hr>";
    }
    if(ui->staticGPS->isChecked()){
        try{
            scenario.eCallStaticGps(filePath, HOST, TARGET_TYPE, DEVICE_IP, duration, report);
        }catch (CommandException& e)
        {
        std::cout << "Simulator Command Exception caught:\n" << e.what() << std::endl;
        }
        catch (std::runtime_error& e)
        {
            std::cout << "Runtime Error Exception caught:\n" << e.what() << std::endl;
        }
        report << "<hr>";
    }
    if(ui->dynamic223->isChecked()){
        try{
            scenario.eCallDynamics223(filePath, HOST, TARGET_TYPE, DEVICE_IP, duration, report);
        }catch (CommandException& e)
        {
        std::cout << "Simulator Command Exception caught:\n" << e.what() << std::endl;
        }
        catch (std::runtime_error& e)
        {
            std::cout << "Runtime Error Exception caught:\n" << e.what() << std::endl;
        }
        report << "<hr>";
    }
    if(ui->dynamic224->isChecked()){
        try{
            scenario.eCallDynamics224(filePath, HOST, TARGET_TYPE, DEVICE_IP, duration, report);            
        }catch (CommandException& e)
        {
        std::cout << "Simulator Command Exception caught:\n" << e.what() << std::endl;
        }
        catch (std::runtime_error& e)
        {
            std::cout << "Runtime Error Exception caught:\n" << e.what() << std::endl;
        }
        report << "<hr>";
    }
    if(ui->ttff2253->isChecked()){
        try{
            scenario.eCallTTFF2253(HOST, TARGET_TYPE, DEVICE_IP, serialPort, nbIteration);
        }catch (CommandException& e)
        {
        std::cout << "Simulator Command Exception caught:\n" << e.what() << std::endl;
        }
        catch (std::runtime_error& e)
        {
            std::cout << "Runtime Error Exception caught:\n" << e.what() << std::endl;
        }
    }
    if(ui->ttff2258->isChecked()){
        try{
            scenario.eCallTTFF2258(HOST, TARGET_TYPE, DEVICE_IP, serialPort, nbIteration);
        }catch (CommandException& e)
        {
        std::cout << "Simulator Command Exception caught:\n" << e.what() << std::endl;
        }
        catch (std::runtime_error& e)
        {
            std::cout << "Runtime Error Exception caught:\n" << e.what() << std::endl;
        }
    }
    if(ui->reAcq226->isChecked()){
        try{
            scenario.eCallReAcq226(HOST, TARGET_TYPE, DEVICE_IP, serialPort, nbIteration);
        }catch (CommandException& e)
        {
        std::cout << "Simulator Command Exception caught:\n" << e.what() << std::endl;
        }
        catch (std::runtime_error& e)
        {
            std::cout << "Runtime Error Exception caught:\n" << e.what() << std::endl;
        }
    }

    report << "</font>";
    report << "</body>";
    report.close();

}

void MainWindow::on_btnConnectReceiver_clicked()
{
    qDebug() << "Connecting to the receiver";
    connect(&serialPort, &SerialPort::dataReceived, this, &MainWindow::readData);
    setupReceiver();
    bool isConnected = serialPort.connect();
    if(!isConnected){
        QMessageBox::critical(this,"Error", "Connection error ! Verify receiver parameters");
    }
}

void MainWindow::on_btnDisconnectReceiver_clicked()
{
    std::cout << "==> Diconnecting to the receiver" << std::endl;
    disconnect(&serialPort, &SerialPort::dataReceived, this, &MainWindow::readData);
    serialPort.disconnect();
}

void MainWindow::readData(QByteArray data)
{
    ui->lstReceiverData->addItem(QString(data));
}

void MainWindow::setupReceiver(){
    serialPort.setupReceiver(ui->cmbReceiver->currentText(), baudRate(), dataBits(), parity(), stopBits(), flowControl());
}

QSerialPort::BaudRate MainWindow::baudRate() {
    QString baudText = ui->cmbBaudRate->currentText();

    if (baudText == "1200") return QSerialPort::Baud1200;
    else if (baudText == "2400") return QSerialPort::Baud2400;
    else if (baudText == "4800") return QSerialPort::Baud4800;
    else if (baudText == "19200") return QSerialPort::Baud19200;
    else if (baudText == "38400") return QSerialPort::Baud38400;
    else if (baudText == "57600") return QSerialPort::Baud57600;
    else if (baudText == "115200") return QSerialPort::Baud115200;

    return QSerialPort::Baud9600;
}

QSerialPort::DataBits MainWindow::dataBits(){
    QString dataBits = ui->cmbDataBits->currentText();

    if (dataBits == "5") return QSerialPort::Data5;
    else if (dataBits == "6") return QSerialPort::Data6;
    else if (dataBits == "7") return QSerialPort::Data7;

    return QSerialPort::Data8;
}

QSerialPort::Parity MainWindow::parity(){
    QString parity = ui->cmbParity->currentText();

    if (parity == "Even") return QSerialPort::EvenParity;
    else if (parity == "Odd") return QSerialPort::OddParity;
    else if (parity == "Space") return QSerialPort::SpaceParity;
    else if (parity == "Mark") return QSerialPort::MarkParity;

    return QSerialPort::NoParity;
}

QSerialPort::FlowControl MainWindow::flowControl(){
    QString flowControl = ui->cmbReceiver->currentText();

    if (flowControl == "Hardware") return QSerialPort::HardwareControl;
    else if (flowControl == "Software") return QSerialPort::SoftwareControl;

    return QSerialPort::NoFlowControl;
}

QSerialPort::StopBits MainWindow::stopBits(){

    if (ui->rBtn1Bit) return QSerialPort::OneStop;

    return QSerialPort::TwoStop;
}

void MainWindow::on_pathBtn_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this,"Choisir un dossier", QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        ui->path->setText(dir);
    }
}


void MainWindow::on_btnRefreshPortList_clicked()
{
    loadPorts();
}

