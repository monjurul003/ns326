/* 
 * File:   GroundLocation.h
 * Author: Israt
 *
 * Created on October 30, 2016, 3:09 PM
 */



//#include "object.h"
#include <ctime>
#include <cmath>
#include <iostream>
#include "ns3/core-module.h"

#define pi 3.14159265358979323846

using namespace ns3;

class MyLocator : public Object {
public:
    MyLocator();
    double calculateCurrentJuleanDate();
    void setCurrentTime();
    double getGreenwichSideRealTime();
    void calculateCurrentPos();
    //<editor-fold defaultstate="collapsed" desc="Getter and Setter">

    Vector getPos() const {
        return pos;
    }

    void setPos(Vector pos) {
        this->pos = pos;
    }

    double getLatitude() const {
        return latitude;
    }

    void setLatitude(double latitude) {
        this->latitude = latitude;
    }

    double getLongitude() const {
        return longitude;
    }

    void setLongitude(double longitude) {
        this->longitude = longitude;
    }

    double getJuleanDate() const {
        return juleanDate;
    }

    double getAltitude() const {
        return altitude;
    }

    void setAltitude(double altitude) {
        this->altitude = altitude;
    }

    //</editor-fold>
private:
    double juleanDate, sec, latitude, longitude, altitude;
    int year, mon, day, hr, minute;
    Vector pos;
};

MyLocator::MyLocator() : juleanDate(0.0) {
    latitude = 32.512027777777774;
    longitude = -105.3886388888888;
    altitude = 0.5;
}

double
MyLocator::calculateCurrentJuleanDate() {
    setCurrentTime();
    juleanDate = 367.0 * year -
            floor((7 * (year + floor((mon + 9) / 12.0))) * 0.25) +
            floor(275 * mon / 9.0) +
            day + 1721013.5 +
            ((sec / 60.0 + minute) / 60.0 + hr) / 24.0;
    return juleanDate;
}

void
MyLocator::setCurrentTime() {
    time_t t = time(0); // get time now
    struct tm * now = localtime(& t);
    year = (now->tm_year + 1900);
    mon = (now->tm_mon + 1);
    day = now->tm_mday;
    hr = now->tm_hour - 1; // deducting 1 as white sands (New Mexico) 1 hour behind Winnipeg
    minute = now->tm_min;
    sec = now->tm_sec;
}

//This function will return the theta_g(t) or the gsmt  

double
MyLocator::getGreenwichSideRealTime() {
    const double twopi = 2.0 * pi;
    const double deg2rad = pi / 180.0;
    double temp, tut1;

    calculateCurrentJuleanDate();
    tut1 = (juleanDate - 2451545.0) / 36525.0;
    temp = -6.2e-6 * tut1 * tut1 * tut1 + 0.093104 * tut1 * tut1 +
            (876600.0 * 3600 + 8640184.812866) * tut1 + 67310.54841; // sec
    temp = fmod(temp * deg2rad / 240.0, twopi); //360/86400 = 1/240, to deg, to rad
//    std::cout << temp << std::endl;
    // ------------------------ check quadrants ---------------------
    if (temp < 0.0)
        temp += twopi;

    return temp;
}

void
MyLocator::calculateCurrentPos() {
    const double twopi = 2.0 * pi;
    double earth_radius = 6378.135; // km
    double theta_g = (getGreenwichSideRealTime() + longitude);

//    std::cout << "Theta_g: " << theta_g << std::endl;
    
    double theta = fmod(theta_g, twopi);
//    std::cout << "Theta: " << theta << std::endl;
    
    double r = (earth_radius + altitude) * cos(latitude);
//    std::cout << "R: " << r << std::endl;
    
    //convert position into meter from km as NS3 support meters
    pos.x = r * cos(theta) *1000;
    pos.y = r * sin(theta)*1000;
    pos.z = (earth_radius + altitude) * sin(latitude)*1000;

}