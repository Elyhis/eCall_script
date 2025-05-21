#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

#include "nmea.h"


//Given a sentence, split it by the chosen delimiter by default ','
// @param string str : string to split
// @return vector<string> : vector of string splitted by ',' delimiter
std::vector<std::string> splitString(const std::string& str) {
    char delimiter = ',';
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}


nmea parser(std::vector<std::string> nmeaData){
    nmea parsed_nmea;
    for(std::string nmeaMessage : nmeaData){
        //Split current message to read each fields
        std::vector<std::string> sentence= splitString(nmeaMessage);
        //Depends on id, push back splitted sentence to it's according vector
        if( nmeaMessage.find("GGA") != std::string::npos){
            parsed_nmea.gga.push_back(sentence);
        }else if (nmeaMessage.find("VTG") != std::string::npos){
            parsed_nmea.vtg.push_back(sentence);
        }else if (nmeaMessage.find("GSA") != std::string::npos){
            parsed_nmea.gsa.push_back(sentence);
        }else if (nmeaMessage.find("GSV") != std::string::npos){
            parsed_nmea.gsv.push_back(sentence);
        }else if (nmeaMessage.find("RMC") != std::string::npos){
            parsed_nmea.rmc.push_back(sentence);
        }else{
            std::cout << "Don't read this type of id" << std::endl;
        }
    }
    return parsed_nmea;
}

std::vector<std::string> reader(const std::string& file){
    std::vector<std::string> nmeaData;
    std::string buffer;
    //Open stream input to read the file
    std::fstream fs;
    fs.open(file);

    //If file is found read each line
    if(fs.is_open()){
        std::cout << "Reading file" << std::endl;
        while(getline(fs,buffer)){
            nmeaData.push_back(buffer);
        }
        fs.close();
    }else{
        std::cout << "File doesn't exist" << std::endl;
    }
    return nmeaData;
}