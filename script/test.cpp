#include <iostream>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <filesystem>

#include <stdio.h>

#define _USE_MATH_DEFINES
#include <math.h>

#define RADIAN(degree) degree / 180.0 * M_PI

#include "all_commands.h"
#include "remote_simulator.h"
#include "lla.h"
#include "attitude.h"

using namespace Sdx;
using namespace Sdx::Cmd;

//Setup sim with same parameter for every test
void setupSim(RemoteSimulator& sim, DateTime& date){
    //  Create new config
    std::cout << "==> Create New Configuration, discarding current simulator settings" << std::endl;
    sim.call(New::create(true));

    // Set default value
    sim.call(SetTropoModel::create("Saastamoinen"));
    sim.call(SetElevationMaskBelow::create(0.0872664625997166682));
    sim.call(SetGpsStartTime::create(date));


    // NMEA log are mandatory for script
    // Every test are based on post treatment NMEA
    sim.call(EnableLogNmea::create(true, false));

    // Set power to JO recommandation
    sim.call(EnableSignalStrengthModel::create(false));
    sim.call(SetGlobalPowerOffset::create(-20));
    sim.call(SetSignalPowerOffset::create("L1CA", -8.5));
    sim.call(SetSignalPowerOffset::create("E1", -5));
}

void setupTrackFromCSV(RemoteSimulator& sim, std::string path){
    //Track
    sim.beginTrackDefinition();

    std::ifstream file(path);
    if(file.is_open()){
        std::string line, word;
        std::getline(file, line);
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::vector<std::string> row;
            while (std::getline(ss, word, ',')) {
                row.push_back(word);
            }
            sim.pushTrackLlaNed(std::stod(row[0]), Lla(RADIAN(std::stod(row[1])), RADIAN(std::stod(row[2])), RADIAN(std::stod(row[3]))), Attitude(RADIAN(std::stod(row[4])),RADIAN(std::stod(row[5])),RADIAN(std::stod(row[6]))));
        }
    }else{
        std::cout << "File doesn't exist" << std::endl;
    }
    file.close();
    int numberOfNodes;
    sim.endTrackDefinition(numberOfNodes);
};

void setupGalileo(RemoteSimulator& sim, bool shouldLoad224){
    //Import satellite constellation
    if(shouldLoad224){
        sim.call(ImportConstellationParameters::create("Galileo", std::filesystem::absolute("../../../eCallData/navigationData/ecall224_GALILEO.txt").string()));
    } else{
        sim.call(ImportConstellationParameters::create("Galileo", std::filesystem::absolute("../../../eCallData/navigationData/ecall223_GALILEO.txt").string()));
    }

    // Setup Galileo
    sim.call(EnableSV::create("Galileo", 0, false));
    sim.call(EnableSV::create("Galileo", 2, true));
    sim.call(EnableSV::create("Galileo", 3, true));
    sim.call(EnableSV::create("Galileo", 21, true));
    sim.call(EnableSV::create("Galileo", 22, true));
    sim.call(EnableSV::create("Galileo", 23, true));
    sim.call(EnableSV::create("Galileo", 24, true));
}

void setupGPS(RemoteSimulator& sim, bool shouldLoad224){
    //Import satellite constellation
    if(shouldLoad224){
        sim.call(ImportConstellationParameters::create("GPS", std::filesystem::absolute("../../../eCallData/navigationData/ecall224_GPS.txt").string()));
    }else{
        sim.call(ImportConstellationParameters::create("GPS", std::filesystem::absolute("../../../eCallData/navigationData/ecall223_GPS.txt").string()));
    }
      
    // Setup GPS
    sim.call(EnableSV::create("GPS", 0, false));
    sim.call(EnableSV::create("GPS", 1, true));
    sim.call(EnableSV::create("GPS", 4, true));
    sim.call(EnableSV::create("GPS", 8, true));
    sim.call(EnableSV::create("GPS", 10, true));
    sim.call(EnableSV::create("GPS", 11, true));
    sim.call(EnableSV::create("GPS", 16, true));
    sim.call(EnableSV::create("GPS", 27, true));
}

void setupSBAS(RemoteSimulator& sim){   
    // Setup SBAS
    sim.call(EnableSV::create("SBAS", 0, false));
    sim.call(EnableSV::create("SBAS", 4, true));
    sim.call(EnableSV::create("SBAS", 17, true));
}

void setupFixPostion(RemoteSimulator& sim){
    double latD = 42.2677;
    double latR = latD * M_PI / 180;
    double lonD = 354.6699;
    double lonR = lonD * M_PI / 180;
    double alt = 830;

    Lla lla = Lla(latR, lonR, alt);

    sim.call(SetVehicleTrajectoryFix::create("Fix", lla.lat, lla.lon, lla.alt, 0, 0, 0));
}

//Test eCallDynamics223 (2.2.3)
void eCallDynamics223(RemoteSimulator& sim , const std::string& targetType, const std::string& X300IP, int& duration){
    std::cout << "=== eCallDynamics223 test ===" << std::endl;

    // Basic setup for the simulation
    // Variable specific to simulation
    DateTime date = DateTime(2017, 10, 4, 10, 0, 0);
    std::string targetId = "MyOutputId";

    setupSim(sim, date);

    //Setup Vehicule
    sim.call(SetVehicleTrajectory::create("Track"));
    setupTrackFromCSV(sim, std::filesystem::absolute("../../../eCallData/trajectories/carEllipse.csv").string());
    sim.call(SetVehicleAntennaGainCSV::create(std::filesystem::absolute("../../../eCallData/antennaModels/Zero-Antenna.csv").string(), AntennaPatternType::Custom, GNSSBand::L1));
    
   // Setup specific satellite
    setupGPS(sim,false);
    setupGalileo(sim,false);
    setupSBAS(sim);


    // Signals
    sim.call(SetModulationTarget::create(targetType, "", "", true, targetId));
    sim.call(ChangeModulationTargetSignals::create(0, 12500000, 100000000, "UpperL", "L1CA,E1,SBASL1", -1, false, targetId));

    // Start simulation
    std::cout << "==> Starting the simulation" << std::endl;
    sim.start();

    // End simulation
    std::cout << "==> Stop simulation when elapsed duration is " << duration << "..." << std::endl;
    sim.stop(duration);
    std::cout << "==> Disconnect from Simulator" << std::endl;
}

//Test eCallDynamics224 (2.2.4)
void eCallDynamics224(RemoteSimulator& sim, const std::string& targetType, const std::string& X300IP, int& duration){
    std::cout << "=== eCallDynamics224 test ===" << std::endl;

    // Basic setup for the simulation
    // Variable specific to simulation
    DateTime date = DateTime(2017, 10, 4, 10, 1, 0);
    std::string targetId = "MyOutputId";

    setupSim(sim, date);

    //Setup Vehicule
    sim.call(SetVehicleAntennaGainCSV::create(std::filesystem::absolute("../../../eCallData/antennaModels/eCall-L1.csv").string(), AntennaPatternType::Custom, GNSSBand::L1));
    sim.call(SetVehicleTrajectory::create("Track"));
    setupTrackFromCSV(sim, std::filesystem::absolute("../../../eCallData/trajectories/carEllipse.csv").string());

    // Change constellation parameters
    setupGPS(sim,true);
    setupGalileo(sim,true);
    setupSBAS(sim);

    //testPDOPwithCustomPos(sim);


    // Signals
    sim.call(SetModulationTarget::create(targetType, "", "", true, targetId));
    sim.call(ChangeModulationTargetSignals::create(0, 12500000, 100000000, "UpperL", "L1CA,E1,SBASL1", -1, false, targetId));

    // Start simulation
    std::cout << "==> Starting the simulation" << std::endl;
    sim.start();

    //Specific interval to shutdown signal
    const double intervalUpInSec = 300.0;
    const double intervalDownInSec = 600.0;
    const double firstTimeStamp = 630.0;
    double timestamp = firstTimeStamp;

    while ((timestamp + intervalDownInSec + intervalUpInSec) < duration)
    {
        sim.call(EnableSignalForSV::create("L1CA",0,false),timestamp);
        sim.call(EnableSignalForSV::create("SBASL1",0,false),timestamp);
        sim.call(EnableSignalForSV::create("E1",0,false),timestamp);
        timestamp += intervalDownInSec;

        sim.call(EnableSignalForSV::create("L1CA",0,true),timestamp);
        sim.call(EnableSignalForSV::create("SBASL1",0,true),timestamp);
        sim.call(EnableSignalForSV::create("E1",0,true),timestamp);
        timestamp += intervalUpInSec;
    }
    // End simulation after specific duration
    std::cout << "==> Stop simulation when elapsed duration is " << duration << "..." << std::endl;
    sim.stop(duration);
    std::cout << "==> Disconnect from Simulator" << std::endl;
}

//Test eCallStatic (2.2.2)
void eCallStatic(RemoteSimulator& sim, const std::string& targetType, const std::string& X300IP, int& duration){
    std::cout << "=== eCallStatic test ===" << std::endl;

    // Basic setup for the simulation
    // Variable specific to simulation
    DateTime date = DateTime(2017, 10, 4, 10, 0, 0);
    std::string targetId = "MyOutputId";

    setupSim(sim, date);

    //Setup Vehicule
    setupFixPostion(sim);
    sim.call(SetVehicleAntennaGainCSV::create(std::filesystem::absolute("../../../eCallData/antennaModels/Zero-Antenna.csv").string(), AntennaPatternType::Custom, GNSSBand::L1));
    
    // Change constellation parameters
    setupGPS(sim,false);
    setupGalileo(sim,false);
    setupSBAS(sim);

    // Signals
    sim.call(SetModulationTarget::create(targetType, "", "", true, targetId));
    sim.call(ChangeModulationTargetSignals::create(0, 12500000, 100000000, "UpperL", "L1CA,E1,SBASL1", -1, false, targetId));

    // Start simulation
    std::cout << "==> Starting the simulation" << std::endl;
    sim.start();
    // End simulation after specific duration
    std::cout << "==> Stop simulation when elapsed duration is " << duration << "..." << std::endl;
    sim.stop(duration);
    std::cout << "==> Disconnect from Simulator" << std::endl;
}

//Test eCallStaticGal (2.2.2.16)
void eCallStaticGal(RemoteSimulator& sim, const std::string& targetType, const std::string& X300IP, int& duration){
    std::cout << "=== eCallStatic test ===" << std::endl;

    // Basic setup for the simulation
    // Variable specific to simulation
    DateTime date = DateTime(2017, 10, 4, 10, 0, 0);
    std::string targetId = "MyOutputId";

    setupSim(sim, date);
    
    //Setup Vehicule
    setupFixPostion(sim);
    sim.call(SetVehicleAntennaGainCSV::create(std::filesystem::absolute("../../../eCallData/antennaModels/Zero-Antenna.csv").string(), AntennaPatternType::Custom, GNSSBand::L1));
    
    // Change constellation parameters
    setupGalileo(sim,false);

    // Signals
    sim.call(SetModulationTarget::create(targetType, "", "", true, targetId));
    sim.call(ChangeModulationTargetSignals::create(0, 12500000, 100000000, "UpperL", "E1", -1, false, targetId));

    // Start simulation
    std::cout << "==> Starting the simulation" << std::endl;
    sim.start();
    // End simulation after specific duration
    std::cout << "==> Stop simulation when elapsed duration is " << duration << "..." << std::endl;
    sim.stop(duration);
    std::cout << "==> Disconnect from Simulator" << std::endl;
}

//Test eCallStaticGPS (2.2.2.15)
void eCallStaticGps(RemoteSimulator& sim, const std::string& targetType, const std::string& X300IP, int& duration){
    std::cout << "=== eCallStaticGPS test ===" << std::endl;

    // Basic setup for the simulation
    // Variable specific to simulation
    DateTime date = DateTime(2017, 10, 4, 10, 0, 0);
    std::string targetId = "MyOutputId";

    setupSim(sim, date);


    //Setup Vehicule
    setupFixPostion(sim);
    sim.call(SetVehicleAntennaGainCSV::create(std::filesystem::absolute("../../../eCallData/antennaModels/Zero-Antenna.csv").string(), AntennaPatternType::Custom, GNSSBand::L1));
    
    // Change constellation parameters
    setupGPS(sim,false);

    // Signals
    sim.call(SetModulationTarget::create(targetType, "", "", true, targetId));
    sim.call(ChangeModulationTargetSignals::create(0, 12500000, 100000000, "UpperL", "L1CA", -1, false, targetId));

    // Start simulation
    std::cout << "==> Starting the simulation" << std::endl;
    sim.start();
    // End simulation after specific duration
    std::cout << "==> Stop simulation when elapsed duration is " << duration << "..." << std::endl;
    sim.stop(duration);
    std::cout << "==> Disconnect from Simulator" << std::endl;
}

//Test eCallTTFF2253 (2.2.5.3)
//TODO:
// Find a way to have fixed position real time with receiver -> Answer : Need to know how to receive receiver data and treat them on my own
// Must check trame GGA to 6 pos and verify to be != 0
void eCallTTFF2253(RemoteSimulator& sim, const std::string& targetType, const std::string& X300IP, int& duration, int nbIteration){
    std::cout << "=== eCallTTFF2253 test ===" << std::endl;

    // Basic setup for the simulation
    // Variable specific to simulation
    DateTime date = DateTime(2017, 10, 4, 10, 0, 0);
    std::string targetId = "MyOutputId";

    setupSim(sim, date);

    //Setup Vehicule
    setupFixPostion(sim);
    sim.call(SetVehicleAntennaGainCSV::create(std::filesystem::absolute("../../../eCallData/antennaModels/Zero-Antenna.csv").string(), AntennaPatternType::Custom, GNSSBand::L1));

    // Change constellation parameters
    setupGPS(sim,false);
    setupGalileo(sim,false);

    // Signals
    sim.call(SetModulationTarget::create(targetType, "", "", true, targetId));
    sim.call(ChangeModulationTargetSignals::create(0, 12500000, 100000000, "UpperL", "L1CA,E1", -1, false, targetId));

    // Start simulation
    for(int i = 0; i < nbIteration; i++){
        do{   
            std::cout << "==> Starting the simulation" << std::endl;
            sim.start();
            // End simulation after specific duration
            std::cout << "==> Stop simulation when elapsed duration is " << duration << "..." << std::endl;
            sim.stop(duration);
            std::cout << "==> Disconnect from Simulator" << std::endl;
    
        }while(true); // TODO: Must check trame GGA to 6 pos and verify to be != 0
    }
}

//Test eCallTTFF2258 (2.2.5.8)
//TODO: all test to do
// Find a way to have fixed position real time with receiver -> Answer : Need to know how to receive receiver data and treat them on my own
// Must check trame GGA to 6 pos and verify to be != 0
void eCallTTFF2258(RemoteSimulator& sim, const std::string& targetType, const std::string& X300IP, int& duration){}