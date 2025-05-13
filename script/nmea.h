#ifndef NMEA_H
#define NMEA_H

#include <vector>
#include <string>

 struct nmea
{
    std::vector<std::vector<std::string>> gga;
    std::vector<std::vector<std::string>> vtg;
    std::vector<std::vector<std::string>> gsa;
    std::vector<std::vector<std::string>> gsv;
    std::vector<std::vector<std::string>> rmc;
};


//Parse nmeaData to an nmea type, split sentence and organize them by id
// @param vector<string> nmeaData : nmea sentence
// @return nmea : nmea struct organize with nmea sentence
nmea parser(std::vector<std::string> nmeaData);

//Read a nmea file, split each sentence into a vector
// @param string file : path to nmea receiver data
// @return vector<string> : vector with nmea sentence
std::vector<std::string> reader(const std::string& file); 

nmea parser(std::vector<std::string> nmeaData);

#endif //ENDIF NMEA_H