#ifndef NMEA_H
#define NMEA_H

#include <vector>
#include <string>
#include <filesystem>

 struct nmea
{
    std::vector<std::vector<std::string>> gga;
    std::vector<std::vector<std::string>> vtg;
    std::vector<std::vector<std::string>> gsa;
    std::vector<std::vector<std::string>> gsv;
    std::vector<std::vector<std::string>> rmc;
};

//Given a sentence, split it by the chosen delimiter by default ','
// @param string str : string to split
// @return vector<string> : vector of string splitted by ',' delimiter
std::vector<std::string> splitString(const std::string& str);


void addTrame(nmea& nmea, std::string nmeaMessage);

//Parse nmeaData to an nmea type, split sentence and organize them by id
// @param vector<string> nmeaData : nmea sentence
// @return nmea : nmea struct organize with nmea sentence
nmea parser(std::vector<std::string> nmeaData);

//Read a nmea file, split each sentence into a vector
// @param string file : path to nmea receiver data
// @return vector<string> : vector with nmea sentence
std::vector<std::string> reader(const std::filesystem::path& file); 

#endif //ENDIF NMEA_H
