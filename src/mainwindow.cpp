#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>

#include <iostream>
#include <tuple>
#include <fstream>

#include "lla.h"
#include "command_exception.h"
#include "date_time.h"


#include "scenario.h"
#include "mathFormulaJO.h"

using namespace Sdx;


// //FIXME: TEMP TO MAKE A GRAPH, WILL BE IN A UI COMPONENT
// void graph(nmea nmea, std::vector<double> horizontalPos){
//     std::vector<std::string> time;
//     std::string filePath = "graph.csv";
//     // file pointer
//     std::fstream fout;
    
//     // opens an existing csv file or creates a new file.
//     fout.open(filePath, std::fstream::out);
//     fout << "Time(UTC),Position error (m)\n";
//     int delta =nmea.rmc.size() - horizontalPos.size();
//     delta = abs(delta);
//     for(int i = 0; i < nmea.rmc.size();i++){
//         if(nmea.rmc[i][2] != "V"){
//             std::string temp = nmea.rmc[i][1];
//             std::string hours = temp.substr(0, 2);  // Get the first two characters for hours
//             std::string mins = temp.substr(2, 2);   // Get the next two characters for minutes
//             std::string sec = temp.substr(4, 2);   // Get the last two characters for seconds
//             std::string t = hours + ":" + mins + ":" + sec ;
//             time.push_back(t);
//         }
//     }
//     for(int i = 0; i < time.size();i++){
//         fout << time[i] << "," << horizontalPos[i] << std::endl;
//     }
//     fout.close();
// }



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //ui->pushButton->isChecked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
   
    std::vector<std::string> nmeaData;
    nmea nmea;

    //FIXME: TEMP VALUE
    double latD = 42.2677;
    double latR = latD * M_PI / 180;
    double lonD = 354.6699 - 360;
    double lonR = lonD * M_PI / 180;
    double alt = 830;

    Lla lla = Lla(latR, lonR, alt);
    // If you wish to connect to the simulator running on a remote computer,
    // change "localhost" for the remote computer's IP address, such as "192.168.1.100"
    const std::string HOST = "localhost";

    //Device type parameters
    const std::string TARGET_TYPE = "None"; // Change to "DTA-2116" to execute on a DTA-2116 device
    const std::string DEVICE_IP = "";           // Change to "192.168.XXX.XXX" to execute on a TARGET_TYPE device
    int duration = 3600; // Time in seconde

    // //Receiver parameters
    // const std::string SERIAL_PORT = "COM5";
    // const int BAUD_RATE = 38400;
    // const SerialPortParity PARITY = SerialPortParity::NoParity;
    // const int DATA_BITS = 8;
    // const int STOP_BITS = 1;
    // const SerialPortFlowControl FLOW_CONTROL = SerialPortFlowControl::NoFlowControl;

    //Path to nmea output data
    //TODO: use GUI to take Skydel NMEA output
    std::string filePath = "C:/Users/Bryan.Barbe/Downloads/250514_090632_2.2.2_gps_gal_sbas_rcv.nmea";
    std::cout << "==> Connecting to the simulator" << std::endl;
    RemoteSimulator sim;
    sim.setVerbose(true);
    sim.connect(HOST);

    // std::cout << "==> Connecting to the receiver" << std::endl;
    // sim.call(ConnectSerialPortReceiver::create(SERIAL_PORT,BAUD_RATE,DATA_BITS,PARITY,STOP_BITS,FLOW_CONTROL));


    if(ui->staticGPS_Gal_SBAB->isChecked()){
        try{
            eCallStatic(sim, TARGET_TYPE, DEVICE_IP, duration);
            nmeaData = reader(filePath);
            nmea = parser(nmeaData);
            // auto pdops = pdopGetter(nmea);
            // bool isPDOPOk = pdopAnalyzer(pdops);
            auto [horizontalPos, mean] = computeHorizontalErrorStats(nmea, lla);

            // auto isHorizontalOK = isHorizontalErrorLessThan15(calculhorizontalPos);
            // auto isField6OK = isField6Correct(nmea.gga);
            //Disconnecting
            // std::cout << "==> Disconnecting to the receiver" << std::endl;
            // sim.call(DisconnectSerialPortReceiver::create());
            std::cout << "Mean value: " << mean << std::endl;
            //graph(nmea, horizontalPos);

            
        }catch (CommandException& e)
        {
        std::cout << "Simulator Command Exception caught:\n" << e.what() << std::endl;
        }
        catch (std::runtime_error& e)
        {
            std::cout << "Runtime Error Exception caught:\n" << e.what() << std::endl;
        }
    }
    std::cout << "==> Connecting to the simulator" << std::endl;
    sim.disconnect();
}
