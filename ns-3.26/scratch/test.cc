
#include "ns3/core-module.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <ctime>
#include <unistd.h>
#include "GroundLocation.h"
#include "satellite-entity.h"

using namespace ns3;

//void positionPrint(Ptr<MyLocator> obj) {
//    std::cout << "Longitude: " << obj->getLongitude() << "\nLatitude: "
//            << obj->getLatitude() << "\nX: " << obj->getPos().x << "\nY: "
//            << obj->getPos().y << "\nZ: " << obj->getPos().z << std::endl;
//}

int
main(int argc, char *argv[]) {
    //    //    ifstream inFile;
    //    //    inFile.open("");
    //    //    
    //    Ptr<MyLocator> obj = CreateObject<MyLocator>();
    //    Ptr<SatelliteLocationEntity> obj2 = CreateObject<SatelliteLocationEntity>();
    //    //    std::cout << obj->calculateCurrentJuleanDate() << std::endl;
    //    //    std::cout << obj->getGreenwichSideRealTime() << std::endl;
    //
    //    std::ifstream fin("/Users/Israt/ns-3-allinone/ns-allinone-3.26/ns-3.26/scratch/tdrs8.txt");
    //
    //    std::vector<std::string> lines;
    //    if (fin.is_open()) {
    //        std::string line;
    //        while (getline(fin, line)) {
    //            lines.push_back(line);
    //            //            obj2->init(line);
    //        }
    //    }
    //    fin.close();
    //
    //
    //    std::cout << "\n vector size--" << lines.size() << std::endl;
    //
    //    for (std::string line : lines) {
    //        obj2->init(line);
    //    }
    //    std::string line;
    //    //    while (!lines.empty()) {
    //    //        line = lines.front();
    //    //        std::cout << line << std::endl;
    //    //        lines.pop_back();
    //    //        std::cout << "After pop :" << lines.size() << std::endl;
    //    //
    //    //    }
    //    //    for (std::string str : lines) {
    //    //        std::cout << str << std::endl;
    //    //    }
    //
    //    
    //    std::ofstream geoLoc;
    //    geoLoc.open("/Users/Israt/ns-3-allinone/ns-allinone-3.26/ns-3.26/scratch/GroundStation.txt");
    //    
    //    std::time_t end = std::time(NULL) + 60.0;
    //    while (std::time(NULL) <= end) {
    //        obj->calculateCurrentPos();
    //        positionPrint(obj);
    //        
    //        
    //        geoLoc<<obj->calculateCurrentJuleanDate()<<" "<<obj->getPos().x<<" "<<obj->getPos().y<<" "<<obj->getPos().z<<"\n";
    //        sleep(1);
    //    }
    //    geoLoc.close();


//    std::string str = "ISS.txt";
//    Ptr<SatelliteEntityManager> entityMng = CreateObject<SatelliteEntityManager>();
//    entityMng->loadVector(str);
//    entityMng->init();
//    entityMng->print();
//    double diff = entityMng->getNextTimeDifferenceWithNextObject() * 100.0;
//    std::cout << "get time difference--" << diff << std::endl;
//    entityMng->getNextObject();
//    entityMng->print();
//    Vector positionVector = entityMng->getcurrentPosition();
//    std::cout << "current position (x,y,z)--" << positionVector.x << "," << positionVector.y << "," << positionVector.z << std::endl;
//
//
//    std::string str1 = "GroundStation.txt";
//    Ptr<GroundLocationEntityManager> entityMng1 = CreateObject<GroundLocationEntityManager>();
//    entityMng1->loadVector(str1);
//    entityMng1->init();
//    entityMng1->print();
//    double diff1 = entityMng1->getNextTimeDifferenceWithNextObject() * 100.0;
//    std::cout << "get time difference--" << diff1 << std::endl;
//    entityMng1->getNextObject();
//    entityMng1->print();
//    positionVector = entityMng1->getcurrentPosition();
//    std::cout << "current position (x,y,z)--" << positionVector.x << "," << positionVector.y << "," << positionVector.z << std::endl;


    return 0;
}

//Time getCurrentTime() {
//
//    time_t t = time(0);   // get time now
//    struct tm * now = localtime( & t );
//    std::cout << (now->tm_year + 1900) << '-' 
//         << (now->tm_mon + 1) << '-'
//         <<  now->tm_mday<<"\t"<<now->tm_hour<<": "<< now->tm_min<<":"<<now->tm_sec
//         << std::endl;
//    return Simulator::Now();
//}
