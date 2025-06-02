#include <stdexcept>
#include <iostream>

#include "ecef.h"
#include "mathFormulaJO.h"

#define _USE_MATH_DEFINES
#include <math.h>

const double SEMI_MAJOR_AXIS = 6378137.0;
const double EXCENTRICITY = 0.0818191908426;
const double ARCSEC_TO_RAD = (M_PI)/(180*3600);

double delta(double value, double trueValue){
    return value - trueValue;
}

double nmeaToDecimal(double val) {
    int deg = (int)(val / 100);
    double min = val - (deg * 100);
    return deg + (min / 60.0);
}

double degToArcsec(double deg) {
    return deg * 3600.0;
}

double systematicInaccuracy(std::vector<double> values){
    double sumDelta = 0;
    for(int i = 0; i < values.size();i++){
        sumDelta += values[i];
    }
    return sumDelta/values.size();
}

double standardDeviation(std::vector<double> values, double mean){
    double result = 0;
    for(double val : values){
        result += pow(val - mean, 2);
    }
    return sqrt(result/(values.size()));
}

double meridianCurve(double phi) {
    return SEMI_MAJOR_AXIS * (1 - pow(EXCENTRICITY,2)) / pow(1 - pow(EXCENTRICITY,2) * pow(sin(phi),2), 1.5);
}

double parallelCurve(double phi) {
    return (SEMI_MAJOR_AXIS * cos(phi)) /sqrt(1 - pow(EXCENTRICITY,2) * pow(sin(phi),2));
}

double latArsecToMeters(double phi, double latArsec) {
    return meridianCurve(phi) * ARCSEC_TO_RAD * latArsec;
}

double lonArsecToMeters(double phi, double lonArcsec) {
    return parallelCurve(phi) * ARCSEC_TO_RAD * lonArcsec;
}

double horizontalPosError(double meanLat, double meanLon, double sigmaBm, double sigmaLm){
    return sqrt(pow(meanLat,2)+pow(meanLon,2))+2*sqrt(pow(sigmaBm,2)+pow(sigmaLm,2));
}

double distance(double first, double second){
    return sqrt(pow(first, 2) + pow(second, 2));
}

std::tuple<std::vector<double>,double> computeHorizontalErrorStats(nmea nmea, Sdx::Lla origin_rad){
    std::vector<double> deltaLats, deltaLons, horizontalError;
    double originLat =origin_rad.latDeg();
    double originLon =origin_rad.lonDeg();
     for(std::vector<std::string> trame : nmea.rmc){
        if(trame.size() < 7 || trame[2] == "V") continue;
        try{
            // Collect value from nmea
            double latNmea = std::stod(trame[3]);
            double lonNmea = std::stod(trame[5]);
            
            // Convert lat and lon to decimal degree
            double latDeg = nmeaToDecimal(latNmea);
            if (trame[4] == "S") latDeg *= -1;
            double lonDeg = nmeaToDecimal(lonNmea);
            if (trame[6] == "W") lonDeg *= -1;
            
            // Convert decimal degree to arcsec degree
            double latArsec = degToArcsec(latDeg);
            double lonArsec = degToArcsec(lonDeg);
            // Calculate lat delta and lon delta between recorded pos and origin pos (origin pos is in rad and need to be converted to arcsec)
            double deltaLat = delta(latArsec, degToArcsec(originLat));
            double deltaLon = delta(lonArsec, degToArcsec(originLon));

            // Convert delta to meters
            deltaLat = latArsecToMeters(origin_rad.lat,deltaLat);
            deltaLon = lonArsecToMeters(origin_rad.lat,deltaLon);
            deltaLats.push_back(deltaLat);
            deltaLons.push_back(deltaLon);

        }
        catch(const std::invalid_argument& e){
            std::cout << "Unexpected value in RMC:\n" << e.what() << std::endl;
        }
    }
    // Calculate mean of values
    double meanLat = systematicInaccuracy(deltaLats);
    double meanLon = systematicInaccuracy(deltaLons);
    // Calculate SD statistics
    double standardDeviationLat = standardDeviation(deltaLats,meanLat);
    double standardDeviationLon = standardDeviation(deltaLons,meanLon);
    // Calculate the overall horizontal position error using Euclidian formula
    for (size_t i = 0; i < deltaLats.size(); i++) {
        double error = distance(deltaLats[i], deltaLons[i]);
        horizontalError.push_back(error);
    }
    // Calculate mean horizontal position error
    double meanHorizontalError = horizontalPosError(meanLat, meanLon, standardDeviationLat, standardDeviationLon);
    return std::make_tuple(horizontalError,meanHorizontalError);
}
