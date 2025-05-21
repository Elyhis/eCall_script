#ifndef SCEANRIO_H
#define SCEANRIO_H

#include <string>

#define RADIAN(degree) degree / 180.0 * M_PI

#include "all_commands.h"
#include "remote_simulator.h"
#include "attitude.h"

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
void eCallDynamics223(RemoteSimulator& sim , const std::string& targetType, const std::string& X300IP, int& duration);

//Test eCallDynamics224 (2.2.4)
void eCallDynamics224(RemoteSimulator& sim, const std::string& targetType, const std::string& X300IP, int& duration);

//Test eCallStatic (2.2.2)
void eCallStatic(RemoteSimulator& sim, const std::string& targetType, const std::string& X300IP, int& duration);

//Test eCallStaticGal (2.2.2.16)
void eCallStaticGal(RemoteSimulator& sim, const std::string& targetType, const std::string& X300IP, int& duration);

//Test eCallStaticGPS (2.2.2.15)
void eCallStaticGps(RemoteSimulator& sim, const std::string& targetType, const std::string& X300IP, int& duration);

//Test eCallTTFF2253 (2.2.5.3)
//TODO:
// Find a way to have fixed position real time with receiver -> Answer : Need to know how to receive receiver data and treat them on my own
// Must check trame GGA to 6 pos and verify to be != 0
void eCallTTFF2253(RemoteSimulator& sim, const std::string& targetType, const std::string& X300IP, int& duration, int nbIteration);

//Test eCallTTFF2258 (2.2.5.8)
//TODO: all test to do
// Find a way to have fixed position real time with receiver -> Answer : Need to know how to receive receiver data and treat them on my own
// Must check trame GGA to 6 pos and verify to be != 0
void eCallTTFF2258(RemoteSimulator& sim, const std::string& targetType, const std::string& X300IP, int& duration, int nbIteration);


#endif //ENDIF SCEANRIO_H