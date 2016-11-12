#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include<cmath>
#include "satellite-entity.h"
#define NO_OF_SOCKETS 3
using namespace ns3;

class SatelliteApp : public Application {
public:

    SatelliteApp();
    virtual ~SatelliteApp();

    void Setup(Ptr<Socket> *socket, Address *address, uint32_t packetSize,
            DataRate dataRate, NodeContainer nodes, Ptr<GroundLocationEntityManager> groundMng,
            Ptr<SatelliteEntityManager> n1Mnger, Ptr<SatelliteEntityManager> n2Mnger,
            Ptr<SatelliteEntityManager> n3Mnger, Ptr<SatelliteEntityManager> issMnger);
    void SetNearestTdrs(int32_t i);
    void SetPosition(double x, double y);
private:
    virtual void StartApplication(void);
    virtual void StopApplication(void);

    void ScheduleTx(void);
    void SchedulePositionChange(void);
    void SatellitePositionChange(Ptr<SatelliteEntityManager> obj, uint16_t id);

    void SendPacket();
    void FindNearestTdrs();

    Ptr<Socket> *m_socket;
    Address m_peer[NO_OF_SOCKETS];
    uint32_t m_packetSize;
    uint64_t m_nPackets;
    uint64_t m_packetsSent;
    uint16_t m_nearestTdrs;
    DataRate m_dataRate;
    EventId m_sendEvent;
    EventId m_issPosEvent;
    EventId m_n1PosEvent;
    EventId m_n2PosEvent;
    EventId m_n3PosEvent;
    bool m_running;

    NodeContainer m_nodes;
    Ptr<OutputStreamWrapper> socketTracker;
    Ptr<GroundLocationEntityManager> m_groundMng;
    Ptr<SatelliteEntityManager> m_n1Mnger;
    Ptr<SatelliteEntityManager> m_n2Mnger;
    Ptr<SatelliteEntityManager> m_n3Mnger;
    Ptr<SatelliteEntityManager> m_issMnger;
};

SatelliteApp::SatelliteApp()
: m_packetSize(0),
m_nPackets(10000000), //9223372036854775800
m_packetsSent(0),
m_nearestTdrs(0),
m_dataRate(0),
m_sendEvent(),
m_issPosEvent(),
m_n1PosEvent(),
m_n2PosEvent(),
m_n3PosEvent(),
m_running(false) {
    m_socket = new Ptr<Socket>[NO_OF_SOCKETS];
    AsciiTraceHelper asciiTraceHelper;
    socketTracker = asciiTraceHelper.CreateFileStream("sockInfo.dat");

};

SatelliteApp::~SatelliteApp() {
    m_socket = 0;
}

void
SatelliteApp::Setup(Ptr<Socket> *socket, Address *address, uint32_t packetSize,
        DataRate dataRate, NodeContainer nodes, Ptr<GroundLocationEntityManager> groundMng,
        Ptr<SatelliteEntityManager> n1Mnger, Ptr<SatelliteEntityManager> n2Mnger,
        Ptr<SatelliteEntityManager> n3Mnger, Ptr<SatelliteEntityManager> issMnger) {
    m_socket = socket;
    for (int i = 0; i < NO_OF_SOCKETS; i++) {
        m_peer[i] = *address;
        address++;
    }
    m_nodes = nodes;
    m_packetSize = packetSize;
    m_dataRate = dataRate;
    m_groundMng = groundMng;
    m_n1Mnger = n1Mnger;
    m_n2Mnger = n2Mnger;
    m_n3Mnger = n3Mnger;
    m_issMnger = issMnger;
}

void
SatelliteApp::SetNearestTdrs(int32_t i) {
    m_nearestTdrs = i;
}

void
SatelliteApp::StartApplication(void) {
    m_running = true;
    FindNearestTdrs();
    SchedulePositionChange();

    m_packetsSent = 0;
    for (int i = 0; i < NO_OF_SOCKETS; i++) {
        m_socket[i]->Bind(); //Allocate a local IPv4 endpoint for this socket. 
        m_socket[i]->Connect(m_peer[i]);
    }
    SendPacket();

}

void
SatelliteApp::StopApplication(void) {
    m_running = false;

    if (m_sendEvent.IsRunning()) {
        Simulator::Cancel(m_sendEvent);
    }
    if (m_issPosEvent.IsRunning()) {
        Simulator::Cancel(m_issPosEvent);
    }

    if (m_n1PosEvent.IsRunning()) {
        Simulator::Cancel(m_n1PosEvent);
    }
    if (m_n2PosEvent.IsRunning()) {
        Simulator::Cancel(m_n2PosEvent);
    }
    if (m_n3PosEvent.IsRunning()) {
        Simulator::Cancel(m_n3PosEvent);
    }

    if (m_socket) {
        for (int i = 0; i < NO_OF_SOCKETS; i++) {
            m_socket[i]->Close();
        }
    }
}

void
SatelliteApp::FindNearestTdrs() {
    //    std::cout << "Sending through TDRS " << sockNumber + 1 << std::endl;
    //ISS mobility
    Ptr<MobilityModel> issMobility = m_nodes.Get(4)->GetObject<MobilityModel>();

    //TDRS1 mobility
    Ptr<MobilityModel> tdrsMobility = m_nodes.Get(1)->GetObject<MobilityModel>();

    double dist1 = issMobility->GetDistanceFrom(tdrsMobility);
    //    std::cout << "distance between tdrs1 and iss " << dist1 << std::endl;
    //tdrs2 
    tdrsMobility = m_nodes.Get(2)->GetObject<MobilityModel>();

    double dist2 = issMobility->GetDistanceFrom(tdrsMobility);
    //    std::cout << "distance between tdrs2 and iss " << dist2 << std::endl;
    //tdrs3 
    tdrsMobility = m_nodes.Get(3)->GetObject<MobilityModel>();
    double dist3 = issMobility->GetDistanceFrom(tdrsMobility);
    //    std::cout << "distance between tdrs3 and iss " << dist3 << std::endl;

    std::cout << "Lowest distance between tdrs and iss is " << std::min(std::min(dist1, dist2), dist3) << std::endl;

    double lowest = std::min(std::min(dist1, dist2), dist3);

    if (lowest == dist1) {
        m_nearestTdrs = 0;

        //                std::cout << "Address between tdrs1 and iss " << addr << std::endl;
    } else if (lowest == dist2) {
        m_nearestTdrs = 1;
        //                std::cout << "Address between tdrs2 and iss " << addr << std::endl;
    } else {
        m_nearestTdrs = 2;
        //                std::cout << "Address between tdrs3 and iss " << addr << std::endl;
    }

    std::cout << "Nearest tdrs from iss is " << m_nearestTdrs + 1 << std::endl;

    *socketTracker->GetStream() << Simulator::Now().GetSeconds() << "\t" << dist1 << "\t" << dist2 << "\t" << dist3 << "\t" << m_nearestTdrs << std::endl;

}

void
SatelliteApp::SendPacket() {
    //    FindNearestTdrs();
    Ptr<Packet> packet = Create<Packet> (m_packetSize);

    m_socket[m_nearestTdrs]->Send(packet);

    if (++m_packetsSent < m_nPackets) {
        ScheduleTx();
    }
}

void
SatelliteApp::SetPosition(double x, double y) {
    Ptr<MobilityModel> mobility = m_nodes.Get(4)->GetObject<MobilityModel> ();
    Vector pos = mobility->GetPosition();
    pos.x = x;
    pos.y = y;
    mobility->SetPosition(pos);
}

void
SatelliteApp::ScheduleTx(void) {
    if (m_running) {
        Time tNext(Seconds(m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate())));
        m_sendEvent = Simulator::Schedule(tNext, &SatelliteApp::SendPacket, this);

        //            Simulator::ScheduleNow(&SatelliteApp::SetPosition, this, x, y);

    }
}

void
SatelliteApp::SchedulePositionChange(void) {

    if (m_running) {

        Time tNext(Seconds(m_issMnger->getNextTimeDifferenceWithNextObject() +0.01));
        m_issPosEvent = Simulator::Schedule(tNext, &SatelliteApp::SatellitePositionChange, this, m_issMnger, 4);
         std::cout << "Tnext: " << tNext.GetSeconds() << std::endl;
//        Time tNext1(tNext.GetSeconds()+ Seconds(0.01));
//        std::cout << "Tnext1: " << tNext1.GetSeconds() << std::endl;
        m_n1PosEvent = Simulator::Schedule(tNext, &SatelliteApp::SatellitePositionChange, this, m_n1Mnger, 1);
//        Time tNext2(tNext.GetSeconds()+ Seconds(0.02));
        m_n2PosEvent = Simulator::Schedule(tNext, &SatelliteApp::SatellitePositionChange, this, m_n2Mnger, 2);
//        Time tNext3(tNext.GetSeconds()+ Seconds(0.03));
        m_n3PosEvent = Simulator::Schedule(tNext, &SatelliteApp::SatellitePositionChange, this, m_n3Mnger, 3);
        FindNearestTdrs();
    }
}

void
SatelliteApp::SatellitePositionChange(Ptr<SatelliteEntityManager> obj, uint16_t id) {
    if (m_running) {

        obj->getNextObject();

        Vector pos = obj->getcurrentPosition();
        Ptr<MobilityModel> mobility = m_nodes.Get(id)->GetObject<MobilityModel> ();
        mobility->SetPosition(pos);
        SchedulePositionChange();
    }
}




