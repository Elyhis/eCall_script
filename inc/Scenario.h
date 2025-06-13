#ifndef SCENARIO_H
#define SCENARIO_H

#include <string>

#define RADIAN(degree) degree / 180.0 * M_PI

#include "all_commands.h"
#include "remote_simulator.h"
#include "attitude.h"

#include "SerialPort.h"
#include "lla.h"
#include "nmea.h"

using namespace Sdx;
using namespace Sdx::Cmd;

class Scenario{
    public:
        //TODO: Add Cold Restart function
        Scenario();
        void eCallStatic(std::filesystem::path filePath, const std::string& HOST, const std::string& TARGET_TYPE, const std::string& DEVICE_IP, int& duration, bool is221Checked, std::fstream &report);
        void eCallStaticGal(std::filesystem::path filePath, const std::string& HOST, const std::string& TARGET_TYPE, const std::string& DEVICE_IP, int& duration, std::fstream& report);
        void eCallStaticGps(std::filesystem::path filePath, const std::string& HOST, const std::string& TARGET_TYPE, const std::string& DEVICE_IP, int& duration);
        void eCallDynamics223(std::filesystem::path filePath, const std::string& HOST, const std::string& TARGET_TYPE, const std::string& DEVICE_IP, int& duration);
        void eCallDynamics224(std::filesystem::path filePath, const std::string& HOST, const std::string& TARGET_TYPE, const std::string& DEVICE_IP, int& duration);
        void eCallTTFF2253(const std::string& HOST, const std::string& TARGET_TYPE, const std::string& DEVICE_IP, SerialPort& receiver, int& nbIteration);
        void eCallTTFF2258(const std::string& HOST, const std::string& TARGET_TYPE, const std::string& DEVICE_IP, SerialPort& receiver, int& nbIteration);
        void eCallReAcq226(const std::string& HOST, const std::string& TARGET_TYPE, const std::string& DEVICE_IP, SerialPort& receiver, int& nbIteration);

    // signals:
    //     void finished();

    private:
        // Position for static test
        Lla fixedPosLla;


        //Setup sim with same parameter for every test
        void setupSim(RemoteSimulator& sim, DateTime& date);
        void setupTrackFromCSV(RemoteSimulator& sim, std::string path);
        void setupGalileo(RemoteSimulator& sim, bool shouldLoad224);
        void setupGPS(RemoteSimulator& sim, bool shouldLoad224);
        void setupSBAS(RemoteSimulator& sim);
        void setupFixPostion(RemoteSimulator& sim);

        //Test eCallStatic (2.2.2)
        void eCallStaticScenario(const std::string& HOST, const std::string& targetType, const std::string& X300IP, int& duration);
        //Test eCallStaticGal (2.2.2.16)
        void eCallStaticGalScenario(const std::string& HOST, const std::string& targetType, const std::string& X300IP, int& duration);
        //Test eCallStaticGPS (2.2.2.15)
        void eCallStaticGpsScenario(const std::string& HOST, const std::string& targetType, const std::string& X300IP, int& duration);
        //Test eCallDynamics223 (2.2.3)
        void eCallDynamics223Scenario(const std::string& HOST, const std::string& targetType, const std::string& X300IP, int& duration);
        //Test eCallDynamics224 (2.2.4)
        void eCallDynamics224Scenario(const std::string& HOST, const std::string& targetType, const std::string& X300IP, int& duration);
        //Test eCallTTFF2253 (2.2.5.3)
        std::chrono::seconds eCallTTFF2253Scenario(const std::string& HOST, const std::string& targetType, const std::string& X300IP, SerialPort& receiver);
        //Test eCallTTFF2258 (2.2.5.8)
        std::chrono::seconds eCallTTFF2258Scenario(const std::string& HOST, const std::string& targetType, const std::string& X300IP, SerialPort& receiver);
        //Test ReAcq226 (2.2.6)
        std::chrono::seconds eCallReAcq226Scenario(const std::string& HOST, const std::string& targetType, const std::string& X300IP, SerialPort& receiver);
};

#endif //ENDIF SCEANRIO_H