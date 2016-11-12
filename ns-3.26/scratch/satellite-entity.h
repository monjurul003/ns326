/* 
 * File:   satellite-entity.h
 * Author: MIK
 *
 * Created on October 31, 2016, 4:20 PM
 */

#ifndef SATELLITE_ENTITY_H
#define	SATELLITE_ENTITY_H

#include "ns3/core-module.h"
#include "ns3/system-mutex.h"
#include<iostream>
#include <fstream>
#include <sstream>

using namespace ns3;

class SatelliteLocationEntity : public Object {
public:

    void init(std::string inputLine) {
        std::istringstream ss(inputLine);
        ss >> time >> positionVector.x >> positionVector.y >> positionVector.z
                >> velocityVector.x >> velocityVector.y >> velocityVector.z
                >> latitude >> longitude >> height >> range >> azimuth >> elevation;
        //        std::cout << " " << time << " \n";

    }




    //<editor-fold defaultstate="collapsed" desc="Getter and Setter">

    double getAzimuth() const {
        return azimuth;
    }

    void setAzimuth(double azimuth) {
        this->azimuth = azimuth;
    }

    double getElevation() const {
        return elevation;
    }

    void setElevation(double elevation) {
        this->elevation = elevation;
    }

    double getHeight() const {
        return height;
    }

    void setHeight(double height) {
        this->height = height;
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

    Vector getPositionVector() const {
        return positionVector;
    }

    void setPositionVector(Vector positionVector) {
        this->positionVector = positionVector;
    }

    double getRange() const {
        return range;
    }

    void setRange(double range) {
        this->range = range;
    }

    double getTime() const {
        return time;
    }

    void setTime(double time) {
        this->time = time;
    }

    Vector getVelocityVector() const {
        return velocityVector;
    }

    void setVelocityVector(Vector velocityVector) {
        this->velocityVector = velocityVector;
    }
    //</editor-fold>
private:
    Vector positionVector, velocityVector;
    double time, latitude, longitude, height, range, azimuth, elevation;
};

class SatelliteEntityManager : public Object {
public:

    SatelliteEntityManager() {
        filePath = "/Users/Israt/ns-3-allinone/ns-allinone-3.26/ns-3.26/scratch/";
         mutex = SystemMutex();
    }

    Vector getcurrentPosition() {
        return currentObj->getPositionVector();
    }

    Ptr<SatelliteLocationEntity> getCurrentObj() {
        return currentObj;
    }

    void init() {
        currentElement = 0;
        currentObj = CreateObject<SatelliteLocationEntity>();
        nextObj = CreateObject<SatelliteLocationEntity>();
        currentObj->init(positionList[currentElement]);
        nextObj->init(positionList[currentElement + 1]);
    }

    void getNextObject() {
        std::cout << "CheckPoint1 "<< std::endl;
        
        {
            std::cout << "CheckPoint2 "<< std::endl;
            CriticalSection cs(mutex);
            std::cout << "CheckPoint3 "<< std::endl;
            currentElement++;
            std::cout << "CheckPoint4 "<< std::endl;
        }

        std::cout << "CurrentElement: " << currentElement << std::endl;

        if (currentElement < positionList.size()) {
            currentObj->init(positionList[currentElement]);
        }
        if (currentElement + 1 < positionList.size()) {
            nextObj->init(positionList[currentElement + 1]);
        }

    }

    double getNextTimeDifferenceWithNextObject() {
        return (nextObj->getTime() - currentObj->getTime());
    }

    void loadVector(std::string fileName) {
        std::ifstream fin(filePath + fileName);
        std::string line;
        if (fin.is_open()) {
            while (getline(fin, line)) {
                positionList.push_back(line);
            }
        }
        fin.close();
    }

    
private:
    SystemMutex mutex;
    std::string filePath;
    std::vector<std::string> positionList;
    int currentElement;
    Ptr<SatelliteLocationEntity> currentObj;
    Ptr<SatelliteLocationEntity> nextObj;
};

class GroundLocationEntity : public Object {
public:

    void init(std::string inputLine);
    //<editor-fold defaultstate="collapsed" desc="Getter and Setter">

    Vector getPositionVector() const {
        return positionVector;
    }

    void setPositionVector(Vector positionVector) {
        this->positionVector = positionVector;
    }

    double getTime() const {
        return time;
    }

    void setTime(double time) {
        this->time = time;
    }

    //</editor-fold>
private:
    Vector positionVector;
    double time;
};

void GroundLocationEntity::init(std::string inputLine) {
    std::istringstream ss(inputLine);
    ss >> time >> positionVector.x >> positionVector.y >> positionVector.z;
}

class GroundLocationEntityManager : public Object {
public:

    GroundLocationEntityManager() {
        filePath = "/Users/Israt/ns-3-allinone/ns-allinone-3.26/ns-3.26/scratch/";
    }

    Vector getcurrentPosition() {
        return currentObj->getPositionVector();
    }

    void init() {
        currentElement = 0;
        currentObj = CreateObject<GroundLocationEntity>();
        currentObj->init(positionList[currentElement]);
    }

    void loadVector(std::string fileName) {
        std::ifstream fin(filePath + fileName);
        std::string line;
        if (fin.is_open()) {
            while (getline(fin, line)) {
                positionList.push_back(line);
            }
        }
        fin.close();
    }

private:

    std::string filePath;
    std::vector<std::string> positionList;
    int currentElement;
    Ptr<GroundLocationEntity> currentObj;
};



#endif	/* SATELLITE_ENTITY_H */



//<editor-fold defaultstate="collapsed" desc="ajaira code>

//  void reAssign(std::string inputLine) {
//        double l_time, l_latitude, l_longitude, l_height, l_range, l_azimuth, l_elevation, rx, ry, rz, vx, vy, vz;
//        std::istringstream ss(inputLine);
//       
//        ss >> l_time >> rx >> ry >> rz
//                >> vx >> vy >> vz
//                >> l_latitude >> l_longitude >> l_height >> l_range >> l_azimuth >> l_elevation;
//
//        time = l_time;
//        positionVector.x = rz;
//        positionVector.y = ry;
//        positionVector.z = rz;
//        velocityVector.x = vx;
//        velocityVector.y = vy;
//        velocityVector.z = vz;
//        latitude = l_latitude;
//        longitude = l_longitude;
//        height = l_height;
//        range = l_range;
//        azimuth = l_azimuth;
//        elevation = l_elevation;
//    }
//</editor-fold>