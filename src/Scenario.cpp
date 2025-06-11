#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <atomic>

#include <QMutex>
#include <QTimer>
#include <QMutexLocker>
#include <QThread>
#include <QObject>
#include <QDebug>
#include <QEventLoop>


#include <filesystem>
#include <stdio.h>

#include "Scenario.h"
#include "mathFormulaJO.h"

using namespace Sdx;
using namespace Sdx::Cmd;

Scenario::Scenario(){
    double latD = 42.2677;
    double latR = RADIAN(latD);
    double lonD = 354.6699 - 360;
    double lonR = RADIAN(lonD);
    double alt = 830;

    fixedPosLla = Lla(latR, lonR, alt);
}
//Setup sim with same parameter for every test
void Scenario::setupSim(RemoteSimulator& sim, DateTime& date){
    //  Create new config
    std::cout << "==> Create New Configuration, discarding current simulator settings" << std::endl;
    sim.call(Open::create(std::filesystem::absolute("../../eCallData/eCall.sdx").string(),true));

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
void Scenario::setupTrackFromCSV(RemoteSimulator& sim, std::string path){
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
void Scenario::setupGalileo(RemoteSimulator& sim, bool shouldLoad224){
    //Import satellite constellation
    if(shouldLoad224){
        sim.call(ImportConstellationParameters::create("GALILEO", std::filesystem::absolute("../../eCallData/navigationData/ecall224_GALILEO.txt").string()));
    } else{
        sim.call(ImportConstellationParameters::create("GALILEO", std::filesystem::absolute("../../eCallData/navigationData/ecall223_GALILEO.txt").string()));
    }

    // Setup Galileo
    sim.call(EnableSV::create("GALILEO", 0, false));
    sim.call(EnableSV::create("GALILEO", 2, true));
    sim.call(EnableSV::create("GALILEO", 3, true));
    sim.call(EnableSV::create("GALILEO", 21, true));
    sim.call(EnableSV::create("GALILEO", 22, true));
    sim.call(EnableSV::create("GALILEO", 23, true));
    sim.call(EnableSV::create("GALILEO", 24, true));
}
void Scenario::setupGPS(RemoteSimulator& sim, bool shouldLoad224){
    //Import satellite constellation
    if(shouldLoad224){
        sim.call(ImportConstellationParameters::create("GPS", std::filesystem::absolute("../../eCallData/navigationData/ecall224_GPS.txt").string()));
    }else{
        sim.call(ImportConstellationParameters::create("GPS", std::filesystem::absolute("../../eCallData/navigationData/ecall223_GPS.txt").string()));
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
void Scenario::setupSBAS(RemoteSimulator& sim){   
    // Setup SBAS
    sim.call(EnableSV::create("SBAS", 0, false));
    sim.call(EnableSV::create("SBAS", 4, true));
    sim.call(EnableSV::create("SBAS", 17, true));
}
void Scenario::setupFixPostion(RemoteSimulator& sim){
    sim.call(SetVehicleTrajectoryFix::create("Fix", fixedPosLla.lat, fixedPosLla.lon, fixedPosLla.alt, 0, 0, 0));
    sim.call(SetVehicleAntennaGainCSV::create(std::filesystem::absolute("../../eCallData/antennaModels/Zero-Antenna.csv").string(), AntennaPatternType::Custom, GNSSBand::L1));
}

// TEST
void Scenario::eCallStatic(std::filesystem::path filePath, const std::string& HOST, const std::string& TARGET_TYPE, const std::string& DEVICE_IP, int& duration, bool is221Checked){
    std::vector<std::string> nmeaData;
    nmea nmea;

    eCallStaticScenario(HOST, TARGET_TYPE, DEVICE_IP, duration);
    nmeaData = reader(filePath);
    nmea = parser(nmeaData);
    auto [horizontalPos, mean] = computeHorizontalErrorStats(nmea, fixedPosLla);

    //bool isHorizontalOK = isHorizontalErrorLessThan15(horizontalPos);
    std::cout << "Mean value: " << mean << std::endl;
    if(is221Checked){
        // bool isField6OK = isField6Correct(nmea.gga);
    }
}
void Scenario::eCallStaticGal(std::filesystem::path filePath, const std::string& HOST, const std::string& TARGET_TYPE, const std::string& DEVICE_IP, int& duration){
    std::vector<std::string> nmeaData;
    nmea nmea;

    eCallStaticGalScenario(HOST, TARGET_TYPE, DEVICE_IP, duration);
    nmeaData = reader(filePath);
    nmea = parser(nmeaData);
    auto [horizontalPos, mean] = computeHorizontalErrorStats(nmea, fixedPosLla);

    //auto isHorizontalOK = isHorizontalErrorLessThan15(horizontalPos);
    std::cout << "Mean value: " << mean << std::endl;
}
void Scenario::eCallStaticGps(std::filesystem::path filePath, const std::string& HOST, const std::string& TARGET_TYPE, const std::string& DEVICE_IP, int& duration){
    std::vector<std::string> nmeaData;
    nmea nmea;

    eCallStaticGpsScenario(HOST, TARGET_TYPE, DEVICE_IP, duration);
    nmeaData = reader(filePath);
    nmea = parser(nmeaData);
    // auto pdops = pdopGetter(nmea);
    // bool isPDOPOk = pdopAnalyzer(pdops);
    auto [horizontalPos, mean] = computeHorizontalErrorStats(nmea, fixedPosLla);

    // auto isHorizontalOK = isHorizontalErrorLessThan15(calculhorizontalPos);
    std::cout << "Mean value: " << mean << std::endl;
}
void Scenario::eCallDynamics223(std::filesystem::path filePath, const std::string& HOST, const std::string& TARGET_TYPE, const std::string& DEVICE_IP, int& duration){
    std::vector<std::string> nmeaData;
    nmea nmea;

    eCallDynamics223Scenario(HOST, TARGET_TYPE, DEVICE_IP, duration);
    nmeaData = reader(filePath);
    nmea = parser(nmeaData);
    // auto pdops = pdopGetter(nmea);
    // bool isPDOPOk = pdopAnalyzer(pdops);
    auto [horizontalPos, mean] = computeHorizontalErrorStats(nmea, fixedPosLla);

    // auto isHorizontalOK = isHorizontalErrorLessThan15(calculhorizontalPos);
    std::cout << "Mean value: " << mean << std::endl;
}
void Scenario::eCallDynamics224(std::filesystem::path filePath, const std::string& HOST, const std::string& TARGET_TYPE, const std::string& DEVICE_IP, int& duration){
    std::vector<std::string> nmeaData;
    nmea nmea;

    std::cout << "==> Connecting to the simulator" << std::endl;
    eCallDynamics224Scenario(HOST, TARGET_TYPE, DEVICE_IP, duration);
    nmeaData = reader(filePath);
    nmea = parser(nmeaData);
    // auto pdops = pdopGetter(nmea);
    // bool isPDOPOk = pdopAnalyzer(pdops);
    auto [horizontalPos, mean] = computeHorizontalErrorStats(nmea, fixedPosLla);

    // auto isHorizontalOK = isHorizontalErrorLessThan15(calculhorizontalPos);
    std::cout << "Mean value: " << mean << std::endl;
}
//FIXME: Test only run once, need to run nbIteration times
void Scenario::eCallTTFF2253(const std::string& HOST, const std::string& TARGET_TYPE, const std::string& DEVICE_IP, SerialPort& receiver, int& nbIteration){
    std::vector<std::chrono::seconds> durations;
    for(int i = 0; i < nbIteration; i++){
        // QEventLoop loop;
        // QObject::connect(this, Scenario::finished, &loop, &QEventLoop::quit);
        std::chrono::seconds duration = eCallTTFF2253Scenario(HOST, TARGET_TYPE, DEVICE_IP, receiver);
        durations.push_back(duration);
        //loop.exec();
        qInfo() << "Iteration " << i << " duration: " << duration.count() << " ms";
    }
    bool isSuccess = false;
    for(std::chrono::seconds time : durations){
        isSuccess |= (time.count() < 20) ;
    }
}
void Scenario::eCallTTFF2258(const std::string& HOST, const std::string& TARGET_TYPE, const std::string& DEVICE_IP, SerialPort& receiver, int& nbIteration){
    auto duration = eCallTTFF2258Scenario(HOST, TARGET_TYPE, DEVICE_IP, receiver);
}

// SCENARIO
//Scenario eCallStatic (2.2.2)
void Scenario::eCallStaticScenario(const std::string& HOST, const std::string& TARGET_TYPE, const std::string& DEVICE_IP, int& duration){
    RemoteSimulator sim;
    sim.setVerbose(true);
    sim.connect(HOST);
    // Basic setup for the simulation
    // Variable specific to simulation
    DateTime date = DateTime(2017, 10, 4, 10, 0, 0);
    std::string targetId = "MyOutputId";

    std::cout << "==> Connecting to the simulator" << std::endl;


    setupSim(sim, date);

    //Setup Vehicule
    setupFixPostion(sim);
    
    // Change constellation parameters
    setupGPS(sim,false);
    setupGalileo(sim,false);
    setupSBAS(sim);

    // Signals
    sim.call(SetModulationTarget::create(TARGET_TYPE, "", "", true, targetId));
    sim.call(ChangeModulationTargetSignals::create(0, 12500000, 100000000, "UpperL", "L1CA,E1,SBASL1", -1, false, targetId));

    // Start simulation
    std::cout << "==> Starting the simulation" << std::endl;
    sim.start();
    // End simulation after specific duration
    sim.stop(duration);
    sim.disconnect();
}
//Test eCallStaticGal (2.2.2.16)
void Scenario::eCallStaticGalScenario(const std::string& HOST, const std::string& TARGET_TYPE, const std::string& DEVICE_IP, int& duration){
    std::cout << "=== eCallStatic test ===" << std::endl;
    
    // Basic setup for the simulation
    // Variable specific to simulation
    DateTime date = DateTime(2017, 10, 4, 10, 0, 0);
    std::string targetId = "MyOutputId";
    std::cout << "==> Connecting to the simulator" << std::endl;
    RemoteSimulator sim;
    sim.setVerbose(true);
    sim.connect(HOST);

    setupSim(sim, date);
    
    //Setup Vehicule
    setupFixPostion(sim);
    
    // Change constellation parameters
    setupGalileo(sim,false);

    // Signals
    sim.call(SetModulationTarget::create(TARGET_TYPE, "", "", true, targetId));
    sim.call(ChangeModulationTargetSignals::create(0, 12500000, 100000000, "UpperL", "E1", -1, false, targetId));

    // Start simulation
    std::cout << "==> Starting the simulation" << std::endl;
    sim.start();
    // End simulation after specific duration
    sim.stop(duration);
    sim.disconnect();

}
//Test eCallStaticGPS (2.2.2.15)
void Scenario::eCallStaticGpsScenario(const std::string& HOST, const std::string& TARGET_TYPE, const std::string& DEVICE_IP, int& duration){
    std::cout << "=== eCallStaticGPS test ===" << std::endl;

    // Basic setup for the simulation
    // Variable specific to simulation
    DateTime date = DateTime(2017, 10, 4, 10, 0, 0);
    std::string targetId = "MyOutputId";
    std::cout << "==> Connecting to the simulator" << std::endl;
    RemoteSimulator sim;
    sim.setVerbose(true);
    sim.connect(HOST);

    setupSim(sim, date);


    //Setup Vehicule
    setupFixPostion(sim);
    
    // Change constellation parameters
    setupGPS(sim,false);

    // Signals
    sim.call(SetModulationTarget::create(TARGET_TYPE, "", "", true, targetId));
    sim.call(ChangeModulationTargetSignals::create(0, 12500000, 100000000, "UpperL", "L1CA", -1, false, targetId));

    // Start simulation
    std::cout << "==> Starting the simulation" << std::endl;
    sim.start();
    // End simulation after specific duration
    sim.stop(duration);
    sim.disconnect();
}

//Test eCallDynamics223 (2.2.3)
void Scenario::eCallDynamics223Scenario(const std::string& HOST , const std::string& TARGET_TYPE, const std::string& DEVICE_IP, int& duration){
    std::cout << "=== eCallDynamics223 test ===" << std::endl;

    // Basic setup for the simulation
    // Variable specific to simulation
    DateTime date = DateTime(2017, 10, 4, 10, 0, 0);
    std::string targetId = "MyOutputId";
    std::cout << "==> Connecting to the simulator" << std::endl;
    RemoteSimulator sim;
    sim.setVerbose(true);
    sim.connect(HOST);

    setupSim(sim, date);

    //Setup Vehicule
    sim.call(SetVehicleTrajectory::create("Track"));
    setupTrackFromCSV(sim, std::filesystem::absolute("../../eCallData/trajectories/carEllipse.csv").string());
    sim.call(SetVehicleAntennaGainCSV::create(std::filesystem::absolute("../../eCallData/antennaModels/Zero-Antenna.csv").string(), AntennaPatternType::Custom, GNSSBand::L1));
    
   // Setup specific satellite
    setupGPS(sim,false);
    setupGalileo(sim,false);
    setupSBAS(sim);


    // Signals
    sim.call(SetModulationTarget::create(TARGET_TYPE, "", "", true, targetId));
    sim.call(ChangeModulationTargetSignals::create(0, 12500000, 100000000, "UpperL", "L1CA,E1,SBASL1", -1, false, targetId));

    // Start simulation
    std::cout << "==> Starting the simulation" << std::endl;
    sim.start();

    // End simulation
    sim.stop(duration);
    sim.disconnect();
}

//Test eCallDynamics224 (2.2.4)
void Scenario::eCallDynamics224Scenario(const std::string& HOST, const std::string& TARGET_TYPE, const std::string& DEVICE_IP, int& duration){
    std::cout << "=== eCallDynamics224 test ===" << std::endl;

    // Basic setup for the simulation
    // Variable specific to simulation
    DateTime date = DateTime(2017, 10, 4, 10, 1, 0);
    std::string targetId = "MyOutputId";
    std::cout << "==> Connecting to the simulator" << std::endl;
    RemoteSimulator sim;
    sim.setVerbose(true);
    sim.connect(HOST);

    setupSim(sim, date);

    //Setup Vehicule
    sim.call(SetVehicleAntennaGainCSV::create(std::filesystem::absolute("../../eCallData/antennaModels/eCall-L1.csv").string(), AntennaPatternType::Custom, GNSSBand::L1));
    sim.call(SetVehicleTrajectory::create("Track"));
    setupTrackFromCSV(sim, std::filesystem::absolute("../../eCallData/trajectories/carEllipse.csv").string());

    // Change constellation parameters
    setupGPS(sim,true);
    setupGalileo(sim,true);
    setupSBAS(sim);

    //testPDOPwithCustomPos(sim);


    // Signals
    sim.call(SetModulationTarget::create(TARGET_TYPE, "", "", true, targetId));
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
    sim.stop(duration);
    sim.disconnect();
}

//Test eCallTTFF2253 (2.2.5.3)
std::chrono::seconds Scenario::eCallTTFF2253Scenario(const std::string& HOST, const std::string& TARGET_TYPE, const std::string& DEVICE_IP, SerialPort& receiver){
    std::cout << "=== eCallTTFF2253 test ===" << std::endl;

    // Basic setup for the simulation
    // Variable specific to simulation
    DateTime date = DateTime(2017, 10, 4, 10, 0, 0);
    std::string targetId = "MyOutputId";
    std::cout << "==> Connecting to the simulator" << std::endl;
    RemoteSimulator sim;
    sim.setVerbose(true);
    sim.connect(HOST);

    setupSim(sim, date);
    //Need to be at -130dBm, must rectify offset of setup
    sim.call(SetSignalPowerOffset::create("L1CA", 0));
    sim.call(SetSignalPowerOffset::create("E1", 0));

    //Setup Vehicule
    setupFixPostion(sim);

    // Change constellation parameters
    setupGPS(sim,false);
    setupGalileo(sim,false);

    // Signals
    sim.call(SetModulationTarget::create(TARGET_TYPE, "", "", true, targetId));
    sim.call(ChangeModulationTargetSignals::create(0, 12500000, 100000000, "UpperL", "L1CA,E1", -1, false, targetId));

    // Start simulation
    QTimer timer;
    std::atomic<bool> isFixed = false;
    QMutex mutex;

    bool test = receiver.connect();
    QObject::connect(&receiver, &SerialPort::dataReceived,
    [&isFixed, &mutex](const QByteArray &data) {
            QMutexLocker locker(&mutex);
            std::vector<std::string> sentence = splitString(QString(data).toStdString());
            if (sentence[0].find("GGA") != std::string::npos && sentence[6] != ("0")) {
                isFixed = true;
            }
        }
    );
    // Lancer la simulation
    qInfo() << "==> Starting the simulation";
    auto start = std::chrono::high_resolution_clock::now();
    sim.start();
    // Boucle Qt temporaire
    QEventLoop loop;
    QTimer pollTimer;

    QObject::connect(&pollTimer, &QTimer::timeout, [&]() {
        QMutexLocker locker(&mutex);
        if (isFixed) {
            qInfo() << "FIX FOUND ";
            loop.quit();  // Sortie propre dès qu’on a un fix
        }
    });
    pollTimer.start(100); // Vérifie toutes les 100 ms
    loop.exec();          // Bloque localement, mais laisse Qt fonctionner

    // Après le fix
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
    qInfo() << "Elapsed time: " << duration.count() << " seconds";

    sim.stop();
    sim.disconnect();
    //emit finished();
    return duration;
}

//Test eCallTTFF2258 (2.2.5.8)
std::chrono::seconds Scenario::eCallTTFF2258Scenario(const std::string& HOST, const std::string& TARGET_TYPE, const std::string& DEVICE_IP, SerialPort& receiver){
    std::cout << "=== eCallTTFF2258 test ===" << std::endl;

    // Basic setup for the simulation
    // Variable specific to simulation
    DateTime date = DateTime(2017, 10, 4, 10, 0, 0);
    std::string targetId = "MyOutputId";
    std::cout << "==> Connecting to the simulator" << std::endl;
    RemoteSimulator sim;
    sim.setVerbose(true);
    sim.connect(HOST);

    setupSim(sim, date);
    //Need to be at -140dBm must rectify offset of setup
    sim.call(SetSignalPowerOffset::create("L1CA", 0));
    sim.call(SetSignalPowerOffset::create("E1", 0));
    sim.call(SetGlobalPowerOffset::create(-30));

    //Setup Vehicule
    setupFixPostion(sim);

    // Change constellation parameters
    setupGPS(sim,false);
    setupGalileo(sim,false);

    // Signals
    sim.call(SetModulationTarget::create(TARGET_TYPE, "", "", true, targetId));
    sim.call(ChangeModulationTargetSignals::create(0, 12500000, 100000000, "UpperL", "L1CA,E1", -1, false, targetId));

    // Start simulation
    QTimer timer;
    std::atomic<bool> isFixed = false;
    QMutex mutex;

    receiver.connect();
    QObject::connect(&receiver, &SerialPort::dataReceived,
    [&isFixed, &mutex](const QByteArray &data) {
            QMutexLocker locker(&mutex);
            std::vector<std::string> sentence = splitString(QString(data).toStdString());
            if (sentence[0].find("GGA") != std::string::npos && sentence[6] != ("0")) {
                isFixed = true;
            }
        }
    );
    // Lancer la simulation
    qInfo() << "==> Starting the simulation";
    auto start = std::chrono::high_resolution_clock::now();
    sim.start();

    // Boucle Qt temporaire
    QEventLoop loop;
    QTimer pollTimer;

    QObject::connect(&pollTimer, &QTimer::timeout, [&]() {
        QMutexLocker locker(&mutex);
        if (isFixed) {
            qInfo() << "FIX FOUND ";
            loop.quit();  // Sortie propre dès qu’on a un fix
        }
    });
    pollTimer.start(100); // Vérifie toutes les 100 ms
    loop.exec();          // Bloque localement, mais laisse Qt fonctionner

    // Après le fix
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
    qInfo() << "Elapsed time: " << duration.count() << " seconds";

    sim.stop();
    sim.disconnect();
    //emit finished();
    return duration;
}

//Test eCallReAcq226 (2.2.6)
// TODO: Need to be tested
std::chrono::seconds Scenario::eCallReAcq226Scenario(const std::string& HOST, const std::string& TARGET_TYPE, const std::string& DEVICE_IP, SerialPort& receiver){
    std::cout << "=== eCallTTFF2253 test ===" << std::endl;
    int stabilizeTime = 900;

    // Basic setup for the simulation
    // Variable specific to simulation
    DateTime date = DateTime(2017, 10, 4, 10, 0, 0);
    std::string targetId = "MyOutputId";
    std::cout << "==> Connecting to the simulator" << std::endl;
    RemoteSimulator sim;
    sim.setVerbose(true);
    sim.connect(HOST);

    setupSim(sim, date);
    //Need to be at -130dBm, must rectify offset of setup
    sim.call(SetSignalPowerOffset::create("L1CA", 0));
    sim.call(SetSignalPowerOffset::create("E1", 0));

    //Setup Vehicule
    setupFixPostion(sim);

    // Change constellation parameters
    setupGPS(sim,false);
    setupGalileo(sim,false);

    // Signals
    sim.call(SetModulationTarget::create(TARGET_TYPE, "", "", true, targetId));
    sim.call(ChangeModulationTargetSignals::create(0, 12500000, 100000000, "UpperL", "L1CA,E1", -1, false, targetId));

    // Start simulation
    QTimer timer;
    std::atomic<bool> isFixed = false;
    QMutex mutex;

    bool test = receiver.connect();
    QObject::connect(&receiver, &SerialPort::dataReceived,
    [&isFixed, &mutex](const QByteArray &data) {
            QMutexLocker locker(&mutex);
            std::vector<std::string> sentence = splitString(QString(data).toStdString());
            if (sentence[0].find("GGA") != std::string::npos && sentence[6] != ("0")) {
                isFixed = true;
            }
        }
    );
    qInfo() << "==> Starting the simulation";
    sim.start();
    qInfo() << "==> Waiting 15 minutes to stabilize receiver";
    QThread::sleep(stabilizeTime);

    auto start = std::chrono::high_resolution_clock::now();
    // Boucle Qt temporaire
    QEventLoop loop;
    QTimer pollTimer;

    QObject::connect(&pollTimer, &QTimer::timeout, [&]() {
        QMutexLocker locker(&mutex);
        if (isFixed) {
            qInfo() << "FIX FOUND ";
            loop.quit();
        }
    });
    pollTimer.start(100);
    loop.exec();

    // Après le fix
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
    qInfo() << "Elapsed time: " << duration.count() << " seconds";

    sim.stop();
    sim.disconnect();
    return duration;
}




// POST TREATMENT
//TODO: Verify if it's mean value or all value
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
    while(i < ggaMessage.size() && ggaMessage[i][6] != "2"){
        i++;
    }
    if(i < ggaMessage.size()){
        std::cout << "\" "  << std::endl;
        for(auto i : ggaMessage[i]){
            std::cout << i << ", ";
        }
        std::cout << " \""  << std::endl;
        return true;
    }
    return false;
}