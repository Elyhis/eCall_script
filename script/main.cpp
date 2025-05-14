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

const int SEMI_MAJOR_AXIS = 6378130;
const double EXCENTRICITY = 0.016;
const double ARCSEC_TO_RAD = (0.5*M_PI)/(180*3600);


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
    std::string filePath = "C:/Users/Bryan.Barbe/OneDrive - Safran Electronics & Defense/Documents/Skydel-SDX/Output/Untitled/nmea_receiver.txt";
    std::vector<std::string> nmeaData;
    nmea nmea;

    std::cout << "==> Connecting to the simulator" << std::endl;
    RemoteSimulator sim;
    sim.setVerbose(true);
    sim.connect(HOST);

    std::cout << "==> Connecting to the receiver" << std::endl;
    sim.call(ConnectSerialPortReceiver::create(SERIAL_PORT,BAUD_RATE,DATA_BITS,PARITY,STOP_BITS,FLOW_CONTROL));

    try{
        eCallStatic(sim, TARGET_TYPE, DEVICE_IP, duration);
        nmeaData = reader(filePath);
        nmea = parser(nmeaData);
        //Disconnecting
        std::cout << "==> Disconnecting to the receiver" << std::endl;
        sim.call(DisconnectSerialPortReceiver::create());

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

//Simple delta formula
double delta(double value, double trueValue){
    return value - trueValue;
}

//Calculate systematic inaccuracy following 2.2.2.7 formula
// @param std::vector<double> values : vector of coordinates to be used
// @param double trueValue : reference value
// @return mean coordinate determination
double systematicInaccuracy(std::vector<double> values, double trueValue){
    double sumDelta = 0;
    for(double value : values){
        sumDelta += delta(value,trueValue);
    }
    return sumDelta/values.size();
}

//Calculate standard deviation following 2.2.2.9 formula
// @param std::vector<double> values : vector of coordinates to be used
// @param double inaccuracy : inaccuracy calculated
// @return standard deviation (SD)
double standardDeviation(std::vector<double> values, double inaccuracy){
    double result = 0;
    for(double val : values){
        result += pow(delta(val,inaccuracy) - inaccuracy, 2);
    }
    return sqrt(result/(values.size()-1));
}

// calculate meridian curve following formula 2.2.2.12 (used in formula 4-1 et 4-2)
double meridianCurve(double phi) {
    double sin_phi = sin(phi);
    return SEMI_MAJOR_AXIS * (1 - pow(EXCENTRICITY,2)) / pow(1 - pow(EXCENTRICITY,2) *pow(sin_phi,2), 1.5);
}

// calculate parallel curve radius following formula 2.2.2.13 (used in formula 5-1 et 5-2)
double parallelCurve(double phi) {
    double sin_phi = sin(phi);
    return (SEMI_MAJOR_AXIS * cos(phi)) /sqrt(1 - pow(EXCENTRICITY,2) *pow(sin_phi,2));
}

// Convert calculated lat from arc-sec to meters (formula 4-1)
double dB_m(double phi, double dB_arcsec) {
    return 2 * meridianCurve(phi) * ARCSEC_TO_RAD * dB_arcsec;
}

// Convert SD lat from arc-sec to meters (formula 4-2)
double sigmaB_m(double phi, double sigmaB_arcsec) {
    return 2 * meridianCurve(phi) * ARCSEC_TO_RAD * sigmaB_arcsec;
}

// Convert calculated lon from arc-sec to meters (formula 5-1)
double dL_m(double phi, double dL_arcsec) {
    return 2 * parallelCurve(phi) * ARCSEC_TO_RAD * dL_arcsec;
}

// Convert SD lat from arc-sec to meters (formula 5-2)
double sigmaL_m(double phi, double sigmaL_arcsec) {
    return 2 * parallelCurve(phi) * ARCSEC_TO_RAD * sigmaL_arcsec;
}

//Formula specified in 2.2.2.14
double horizontalPosError(double lat, double latInaccuracy, double lonInaccuracy, double sdLat, double sdLon){
    return sqrt(pow(dB_m(lat,latInaccuracy),2)+pow(dL_m(lat,lonInaccuracy),2))+2*sqrt(pow(sigmaB_m(lat,sdLat),2)+pow(sigmaL_m(lat,sdLon),2));
}

std::vector<double> calculhorizontalPosError(nmea nmea, Lla originLla){
    std::vector<double> latitudes;
    std::vector<double> longitudes;
    std::vector<double> horizontalPos;
    //Get all latitudes and longitudes recorded
    for(std::vector<std::string> trame : nmea.gga){
        double lat = std::stod(trame[2]);
        double lon = std::stod(trame[4]);
        latitudes.push_back(lat);
        longitudes.push_back(lon);
        //Calculate inaccuracy following JO formula
        double inaccuracyLat = systematicInaccuracy(latitudes, originLla.lat);
        double inaccuracyLon = systematicInaccuracy(longitudes, originLla.lon);
        double sdLat = standardDeviation(latitudes,inaccuracyLat);
        double sdLon = standardDeviation(longitudes,inaccuracyLon);
        horizontalPos.push_back(horizontalPosError(lat,inaccuracyLat,inaccuracyLon,sdLat,sdLon));
    }
    return horizontalPos;
}

bool isHorizontalErrorLessThan15(std::vector<double> horizontalPos){
    int threshold = 15;
    //Checking if receiver pass the test or not
    bool isLessThan15Meters = true;
    for(double value : horizontalPos){
        if (value > threshold) isLessThan15Meters = false;
    }
    return isLessThan15Meters;
}