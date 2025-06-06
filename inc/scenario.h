#ifndef SCEANRIO_H
#define SCEANRIO_H

#include <string>

#define RADIAN(degree) degree / 180.0 * M_PI

#include "all_commands.h"
#include "remote_simulator.h"
#include "attitude.h"

#include "SerialPort.h"

using namespace Sdx;
using namespace Sdx::Cmd;

//Setup sim with same parameter for every test
void setupSim(RemoteSimulator& sim, DateTime& date);

void setupTrackFromCSV(RemoteSimulator& sim, std::string path);

void setupGalileo(RemoteSimulator& sim, bool shouldLoad224);

void setupGPS(RemoteSimulator& sim, bool shouldLoad224);

void setupSBAS(RemoteSimulator& sim);

void setupFixPostion(RemoteSimulator& sim);

//Test eCallDynamics223 (2.2.3)
void eCallDynamics223Scenario(RemoteSimulator& sim , const std::string& targetType, const std::string& X300IP, int& duration);

//Test eCallDynamics224 (2.2.4)
void eCallDynamics224Scenario(RemoteSimulator& sim, const std::string& targetType, const std::string& X300IP, int& duration);

//Test eCallStatic (2.2.2)
void eCallStaticScenario(RemoteSimulator& sim, const std::string& targetType, const std::string& X300IP, int& duration);

//Test eCallStaticGal (2.2.2.16)
void eCallStaticGalScenario(RemoteSimulator& sim, const std::string& targetType, const std::string& X300IP, int& duration);

//Test eCallStaticGPS (2.2.2.15)
void eCallStaticGpsScenario(RemoteSimulator& sim, const std::string& targetType, const std::string& X300IP, int& duration);

//Test eCallTTFF2253 (2.2.5.3)
void eCallTTFF2253Scenario(RemoteSimulator& sim, const std::string& targetType, const std::string& X300IP, int nbIteration, SerialPort& receiver);

//Test eCallTTFF2258 (2.2.5.8)
void eCallTTFF2258Scenario(RemoteSimulator& sim, const std::string& targetType, const std::string& X300IP, int nbIteration, SerialPort& receiver);

//Test eCallTTFF2258 (2.2.5.8)
void eCallReAcq226(RemoteSimulator& sim, const std::string& targetType, const std::string& X300IP, int nbIteration, SerialPort& receiver);

#endif //ENDIF SCEANRIO_H