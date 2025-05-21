#ifndef MATH_FORMULA_JO_H
#define MATH_FORMULA_JO_H

#include <vector>

#define _USE_MATH_DEFINES
#include <math.h>

#include "nmea.h"
#include "lla.h"

const double SEMI_MAJOR_AXIS = 6378137.0;
const double EXCENTRICITY = 0.0818191908426;
const double ARCSEC_TO_RAD = (M_PI)/(180*3600);


//Simple delta formula
double delta(double value, double trueValue);

// Convert nmea data to decimal angle
double nmeaToDecimal(double val);

// Convert decimal angle to arcsec
double degToArcsec(double deg);

// Calculate mean of systematic inaccuracy
// @param std::vector<double> values : vector of coordinates to be used
// @param double trueValue : reference value
// @return mean coordinate determination
double systematicInaccuracy(std::vector<double> values);

// Calculate standard deviation
// @param std::vector<double> values : vector of coordinates to be used
// @param double inaccuracy : inaccuracy calculated
// @return standard deviation (SD)
double standardDeviation(std::vector<double> values, double mean);

// calculate meridian curve radius
// @param double phi : true lat used in the simulation
// @return meridian curve radius
double meridianCurve(double phi);

// calculate parallel curve radius
// @param double phi : true lat used in the simulation
double parallelCurve(double phi);

// Convert calculated lat from arc-sec to meters
// @param double phi : true lat used in the simulation
// @param double latArsec : calculated systematic inaccuracy at the given lat
double latArsecToMeters(double phi, double latArsec);
// Convert calculated lon from arc-sec to meters
// @param double phi : true lat used in the simulation
// @param double lonArsec : calculated systematic inaccuracy at the given lon
double lonArsecToMeters(double phi, double lonArcsec);

// Calculate the overall horizontal position error
// @param double meanLat: mean latitude in meters
// @param double meanLon: mean longitude in meters
// @param double sigmaBm: standard deviation of the latitude
// @param double sigmaLm: standard deviation of the longitude
// @return horizontal position error using COMMISSION DELEGATED REGULATION formula
double horizontalPosError(double meanLat, double meanLon, double sigmaBm, double sigmaLm);
//Calculate distance between two point
double distance(double first, double second);

std::tuple<std::vector<double>,double> computeHorizontalErrorStats(nmea nmea, Sdx::Lla origin_rad);


#endif //ENDIF MATH_FORMULA_JO_H