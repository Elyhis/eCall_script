#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "nmea.h"
#include "test.cpp"

#include "lla.h"
#include "command_exception.h"
#include "date_time.h"

using namespace Sdx;

int main(){
    // If you wish to connect to the simulator running on a remote computer,
    // change "localhost" for the remote computer's IP address, such as "192.168.1.100"
    const std::string HOST = "localhost";

    //Device type
    const std::string TARGET_TYPE = "None"; // Change to "X300" to execute on a X300 device
    const std::string X300_IP = "";           // Change to "192.168.XXX.XXX" to execute on a X300 device
    int duration = 60; // Time in seconde

    //Receiver
    const std::string SERIAL_PORT = "COM5";
    const int BAUD_RATE = 38400;
    const SerialPortParity PARITY = SerialPortParity::NoParity;
    const int DATA_BITS = 8;
    const int STOP_BITS = 1;
    const SerialPortFlowControl FLOW_CONTROL = SerialPortFlowControl::NoFlowControl;

    //Path to nmea output data
    //TODO: use GUI to take Skydel NMEA output
    std::string filePath = "C:/Users/Bryan.Barbe/OneDrive - Safran Electronics & Defense/Documents/Skydel-SDX/Output/Untitled/nmea_receiver.txt";
    std::vector<std::string> nmeaData;
    nmea nmea;

    std::cout << "==> Connecting to the simulator" << std::endl;
    RemoteSimulator sim;
    sim.setVerbose(true);
    sim.connect(HOST);

    std::cout << "==> Connecting to the receiver" << std::endl;
    //sim.call(ConnectSerialPortReceiver::create(SERIAL_PORT,BAUD_RATE,DATA_BITS,PARITY,STOP_BITS,FLOW_CONTROL));

    try{
        eCallDynamics223(sim, TARGET_TYPE, X300_IP, duration);
        nmeaData = reader(filePath);
        nmea = parser(nmeaData);
        //Disconnecting
        std::cout << "==> Disconnecting to the receiver" << std::endl;
        //sim.call(DisconnectSerialPortReceiver::create());

        std::cout << "==> Connecting to the simulator" << std::endl;
        sim.disconnect();
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