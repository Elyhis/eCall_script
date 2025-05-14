#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "nmea.h"
#include "scenario.cpp"
#include "mathFormulaJO.cpp"

#include "lla.h"
#include "command_exception.h"
#include "date_time.h"

using namespace Sdx;

bool isHorizontalErrorLessThan15(std::vector<double> horizontalPos){
    int threshold = 15;
    //Checking if receiver pass the test or not
    bool isLessThan15Meters = true;
    for(double value : horizontalPos){
        if (value > threshold) isLessThan15Meters = false;
    }
    return isLessThan15Meters;
}
//2.2.1 test to check if there is a message with value 2 at 6th position
bool isField6Correct(std::vector<std::vector<std::string>> ggaMessage){
    int i = 0;
    while(ggaMessage[i][6].compare("2") && i < ggaMessage.size()){
        if(ggaMessage[i][6].compare("2")) return true;
        i++;
    }
    return false;
}

int main(){
    // If you wish to connect to the simulator running on a remote computer,
    // change "localhost" for the remote computer's IP address, such as "192.168.1.100"
    const std::string HOST = "localhost";

    //Device type parameters
    const std::string TARGET_TYPE = "None"; // Change to "X300" to execute on a X300 device
    const std::string DEVICE_IP = "";           // Change to "192.168.XXX.XXX" to execute on a X300 device
    int duration = 3600; // Time in seconde

    //Receiver parameters
    const std::string SERIAL_PORT = "COM5";
    const int BAUD_RATE = 38400;
    const SerialPortParity PARITY = SerialPortParity::NoParity;
    const int DATA_BITS = 8;
    const int STOP_BITS = 1;
    const SerialPortFlowControl FLOW_CONTROL = SerialPortFlowControl::NoFlowControl;

    //Path to nmea output data
    //TODO: use GUI to take Skydel NMEA output
    std::string filePath = "C:/Users/Bryan.Barbe/Downloads/250514_090632_2.2.2_gps_gal_sbas_rcv.nmea";
    std::vector<std::string> nmeaData;
    nmea nmea;

    //FIXME: TEMP VALUE
    double latD = 42.2677;
    double latR = latD * M_PI / 180;
    double lonD = 354.6699 - 360;
    double lonR = lonD * M_PI / 180;
    double alt = 830;

    Lla lla = Lla(latR, lonR, alt);

    // std::cout << "==> Connecting to the simulator" << std::endl;
    // RemoteSimulator sim;
    // sim.setVerbose(true);
    // sim.connect(HOST);

    // std::cout << "==> Connecting to the receiver" << std::endl;
    //sim.call(ConnectSerialPortReceiver::create(SERIAL_PORT,BAUD_RATE,DATA_BITS,PARITY,STOP_BITS,FLOW_CONTROL));

    try{
        //eCallStatic(sim, TARGET_TYPE, DEVICE_IP, duration);
        std::cout << "Testing parser" << std::endl;

        nmeaData = reader(filePath);
        nmea = parser(nmeaData);
        auto calculhorizontalPos = calculhorizontalPosError(nmea,lla);
        auto isHorizontalOK = isHorizontalErrorLessThan15(calculhorizontalPos);
        //Disconnecting
        // std::cout << "==> Disconnecting to the receiver" << std::endl;
        // sim.call(DisconnectSerialPortReceiver::create());

        // std::cout << "==> Connecting to the simulator" << std::endl;
        // sim.disconnect();
    }catch (CommandException& e)
    {
    std::cout << "Simulator Command Exception caught:\n" << e.what() << std::endl;
    }
    catch (std::runtime_error& e)
    {
        std::cout << "Runtime Error Exception caught:\n" << e.what() << std::endl;
    }
    return 0;
}