/* 
 * File:   satellite-entity.h
 * Author: MIK
 *
 * Created on October 31, 2016, 4:20 PM
 */

#ifndef SATELLITE_ENTITY_H
#define	SATELLITE_ENTITY_H

#include "ns3/core-module.h"
#include<iostream>
#include <fstream>
#include <sstream>



    class SatelliteLocationEntity : public ns3::Object {
    public:

        void init(std::string inputLine);


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

        ns3::Vector getPositionVector() const {
            return positionVector;
        }

        void setPositionVector(ns3::Vector positionVector) {
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

        ns3::Vector getVelocityVector() const {
            return velocityVector;
        }

        void setVelocityVector(ns3::Vector velocityVector) {
            this->velocityVector = velocityVector;
        }
        //</editor-fold>
    private:
        ns3::Vector positionVector, velocityVector;
        double time, latitude, longitude, height, range, azimuth, elevation;
    };

    void
    SatelliteLocationEntity::init(std::string inputLine) {
        std::istringstream ss(inputLine);
        ss >> time >> positionVector.x >> positionVector.y >> positionVector.z
                >> velocityVector.x >> velocityVector.y >> velocityVector.z
                >> latitude >> longitude >> height >> range >> azimuth >> elevation;
        //        <editor-fold defaultstate="collapsed" desc="printf">
        //        std::cout << " " << time << " " << positionVector.x << " " << positionVector.y << " " << positionVector.z
        //                << " " << velocityVector.x << " " << velocityVector.y << " " << velocityVector.z
        //                << " " << latitude << " " << longitude << " " << height << " " << range << " " << azimuth << " " << elevation << std::endl;

        //        while (fin >> time >> rx >> ry >> rz
        //                >> vx >> vy >> vz
        //                >> latitude >> longitude >> height >> range >> azimuth >> elevation) {
        //
        //            std::cout << " " << time << " " << rx << " " << ry << " " << rz
        //                    << " " << vx << " " << vy << " " << vz
        //                    << " " << latitude << " " << longitude << " " << height << " " << range << " " << azimuth << " " << elevation << std::endl;
        //
        //        }
        //</editor-fold>

    }

    class SatelliteEntityManager : public ns3::Object {
    public:

        SatelliteEntityManager() {
            filePath = "/Users/Israt/ns-3-allinone/ns-allinone-3.26/ns-3.26/scratch/";
        }

        void init() {
            currentElement = 0;
            currentObj = ns3::CreateObject<SatelliteLocationEntity>();
            nextObj = ns3::CreateObject<SatelliteLocationEntity>();
            currentObj->init(positionList[currentElement]);
            nextObj->init(positionList[currentElement + 1]);
        }

        void getNextObject() {
            currentElement++;
            currentObj->init(positionList[currentElement]);
            if (currentElement + 1 < positionList.size()) {
                nextObj->init(positionList[currentElement + 1]);
            }

        }

        double getNextTimeDifferenceWithNextObject() {
            double nextT = nextObj->getTime();
            double currT = currentObj->getTime();
            double diff = nextT - currT;
            std::cout << "time diff " << nextT << "-" << currT << "= " << diff << std::endl;
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

        ns3::Vector getCurrentPosition() {
            currentObj->getPositionVector();
        }

        void print() {
            std::cout << "current time--" << currentObj->getTime() << std::endl;
        }
    private:

        std::string filePath;
        std::vector<std::string> positionList;
        int currentElement;
        ns3::Ptr<SatelliteLocationEntity> currentObj;
        ns3::Ptr<SatelliteLocationEntity> nextObj;
    };

    class GroundLocationEntity : public ns3::Object {
    public:

        void init(std::string inputLine);
        //<editor-fold defaultstate="collapsed" desc="Getter and Setter">

        ns3::Vector getPositionVector() const {
            return positionVector;
        }

        void setPositionVector(ns3::Vector positionVector) {
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
        ns3::Vector positionVector;
        double time;
    };

    void GroundLocationEntity::init(std::string inputLine) {
        std::istringstream ss(inputLine);
        ss >> time >> positionVector.x >> positionVector.y >> positionVector.z;
    }

    class GroundLocationEntityManager : public ns3::Object {
    public:

        GroundLocationEntityManager() {
            filePath = "/Users/Israt/ns-3-allinone/ns-allinone-3.26/ns-3.26/scratch/";
        }

        void init() {
            currentElement = 0;
            currentObj = ns3::CreateObject<GroundLocationEntity>();
            nextObj = ns3::CreateObject<GroundLocationEntity>();
            currentObj->init(positionList[currentElement]);
            nextObj->init(positionList[currentElement + 1]);
        }

        void getNextObject() {
            currentElement++;
            currentObj->init(positionList[currentElement]);
            if (currentElement + 1 < positionList.size()) {
                nextObj->init(positionList[currentElement + 1]);
            }

        }

        double getNextTimeDifferenceWithNextObject() {
            double nextT = nextObj->getTime();
            double currT = currentObj->getTime();
            double diff = nextT - currT;
            std::cout << "time diff " << nextT << "-" << currT << "= " << diff << std::endl;
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

        ns3::Vector getCurrentPosition() {
            currentObj->getPositionVector();
        }

        void print() {
            std::cout << "current time--" << currentObj->getTime() << std::endl;
        }
    private:

        std::string filePath;
        std::vector<std::string> positionList;
        int currentElement;
        ns3::Ptr<GroundLocationEntity> currentObj;
        ns3::Ptr<GroundLocationEntity> nextObj;
    };


#endif	/* SATELLITE_ENTITY_H */

