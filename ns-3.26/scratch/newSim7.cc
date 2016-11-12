#include <cmath>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/satellite-point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "satellite-entity.h"
#include "satellite-app.h"

//Network Topology
//   TDRS8(n2)----TDRS9(n1)
//      |        /   |   
//      |       /    |
//      |      /     |
//   ISS(n4)  /      |
//           /       |
//          /        |
//         /         |
//       n0          |
//                   |
//                   |
//                   |
//                   |
//                   TDSR10(n3) 


using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Simulation7");


uint64_t droppedPkt = 0;

//<editor-fold defaultstate="collapsed" desc="Trace Function decalration and definition">

static void
CwndChange(Ptr<OutputStreamWrapper> cWndStream, uint32_t oldval, uint32_t newval) {

    *cWndStream->GetStream() << Simulator::Now().GetMilliSeconds() << "\t" << newval << std::endl;
}

static void
RxDrop(Ptr<PcapFileWrapper> file, Ptr<const Packet> p) {
    //    NS_LOG_UNCOND("RxDrop at " << Simulator::Now().GetSeconds());
    file->Write(Simulator::Now(), p);
    droppedPkt++;
}
//</editor-fold>

int main(int argc, char* argv[]) {

    Time::SetResolution(Time::MS);


    //<editor-fold defaultstate="collapsed" desc="Node, device and channel creation" >
    NodeContainer satNodes, n0n1, n1n2, n1n3, n1n4, n2n4, n3n4;
    satNodes.Create(5);

    n0n1.Add(satNodes.Get(0)); //add the earth station
    n0n1.Add(satNodes.Get(1)); //add the TDRS9

    n1n2.Add(satNodes.Get(1)); //add the TDRS9
    n1n2.Add(satNodes.Get(2)); //add the TDRS2

    n1n3.Add(satNodes.Get(1)); //add the TDRS9
    n1n3.Add(satNodes.Get(3)); //add the TDRS3

    n1n4.Add(satNodes.Get(1)); //add the TDRS9
    n1n4.Add(satNodes.Get(4)); //add the ISS

    n2n4.Add(satNodes.Get(2)); //add the TDRS8
    n2n4.Add(satNodes.Get(4)); //add the ISS

    n3n4.Add(satNodes.Get(3)); //add the TDRS10
    n3n4.Add(satNodes.Get(4)); //add the ISS


    SatellitePointToPointHelper sathelper;
    sathelper.SetDeviceAttribute("DataRate", StringValue("2Mbps"));

    //Topology

    NetDeviceContainer deviceN0N1, deviceN1N2, deviceN1N3;
    deviceN0N1 = sathelper.Install(n0n1);
    deviceN1N2 = sathelper.Install(n1n2);
    deviceN1N3 = sathelper.Install(n1n3);

    NetDeviceContainer deviceN1N4, deviceN2N4, deviceN3N4;
    deviceN1N4 = sathelper.Install(n1n4);
    deviceN2N4 = sathelper.Install(n2n4);
    deviceN3N4 = sathelper.Install(n3n4);

    //</editor-fold>

    //<editor-fold defaultstate="collapsed" desc="Addressing and other stuff">
    InternetStackHelper stack;

    //internet stack should be installed once in one node
    stack.Install(satNodes);

    Ipv4AddressHelper address;


    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaceN0N1;
    interfaceN0N1 = address.Assign(deviceN0N1); // So Earth-> 10.1.1.1, TDRS1 -> 10.1.1.2

    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaceN1N2;
    interfaceN1N2 = address.Assign(deviceN1N2); // So TDRS1 -> 10.1.2.1, TDRS2 -> 10.1.2.2

    address.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaceN1N3;
    interfaceN1N3 = address.Assign(deviceN1N3); // So TDRS1 -> 10.1.3.1, TDRS3 -> 10.1.3.2

    address.SetBase("10.1.4.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaceN1N4;
    interfaceN1N4 = address.Assign(deviceN1N4); // So TDRS1 -> 10.1.4.1, ISS -> 10.1.4.2

    address.SetBase("10.1.5.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaceN2N4;
    interfaceN2N4 = address.Assign(deviceN2N4); // So TDRS2 -> 10.1.5.1, ISS -> 10.1.5.2

    address.SetBase("10.1.6.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaceN3N4;
    interfaceN3N4 = address.Assign(deviceN3N4); // So TDRS3 -> 10.1.6.1, ISS -> 10.1.6.2

    //</editor-fold>

    //<editor-fold defaultstate="collapsed" desc="Mobility and position allocation">
    MobilityHelper mobility;

    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
            "MinX", DoubleValue(0.0),
            "MinY", DoubleValue(0.0),
            "DeltaX", DoubleValue(1000.0),
            "DeltaY", DoubleValue(1000.0),
            "GridWidth", UintegerValue(3),
            "LayoutType", StringValue("ColumnFirst"));

    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(satNodes);

    // Earth base station position
    std::string fileName = "GroundStation.txt";
    Ptr<GroundLocationEntityManager> groundMng = CreateObject<GroundLocationEntityManager>();
    groundMng->loadVector(fileName);
    groundMng->init();
    Vector positionVector = groundMng->getcurrentPosition();
    Ptr<Node> tmpNode = satNodes.Get(0);
    Ptr<MobilityModel> mob = tmpNode->GetObject<MobilityModel>();
    mob->SetPosition(positionVector);


    //n1 -- TDRS9 position

    fileName = "tdrs9.txt";
    Ptr<SatelliteEntityManager> n1tdrs9Mng = CreateObject<SatelliteEntityManager>();
    n1tdrs9Mng->loadVector(fileName);
    n1tdrs9Mng->init();
    positionVector = n1tdrs9Mng->getcurrentPosition();
    tmpNode = satNodes.Get(1);
    mob = tmpNode->GetObject<MobilityModel>();
    mob->SetPosition(positionVector);

    //n2 -- TDRS8 position

    fileName = "tdrs8.txt";
    Ptr<SatelliteEntityManager> n2tdrs8Mng = CreateObject<SatelliteEntityManager>();
    n2tdrs8Mng->loadVector(fileName);
    n2tdrs8Mng->init();
    positionVector = n2tdrs8Mng->getcurrentPosition();
    tmpNode = satNodes.Get(2);
    mob = tmpNode->GetObject<MobilityModel>();
    mob->SetPosition(positionVector);



    //n3 -- TDRS10 position

    fileName = "tdrs10.txt";
    Ptr<SatelliteEntityManager> n3tdrs10Mng = CreateObject<SatelliteEntityManager>();
    n3tdrs10Mng->loadVector(fileName);
    n3tdrs10Mng->init();
    positionVector = n3tdrs10Mng->getcurrentPosition();
    tmpNode = satNodes.Get(3);
    mob = tmpNode->GetObject<MobilityModel>();
    mob->SetPosition(positionVector);

    //n4 -- ISS position

    fileName = "ISS.txt";
    Ptr<SatelliteEntityManager> issMng = CreateObject<SatelliteEntityManager>();
    issMng->loadVector(fileName);
    issMng->init();
    positionVector = issMng->getcurrentPosition();
    tmpNode = satNodes.Get(4);
    mob = tmpNode->GetObject<MobilityModel>();
    mob->SetPosition(positionVector);
    
     
    
    
    //</editor-fold>

    //<editor-fold defaultstate="collapsed" desc="Sink app creation in the ISS" >
    // sink port creation 
    uint16_t sinkPort = 8080;

    // Create a sink application at ISS listening to port 8080 binded with IP address 0.0.0.0
    PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), sinkPort));
    ApplicationContainer sinkApps = packetSinkHelper.Install(satNodes.Get(4));

    sinkApps.Start(Seconds(0.));
    sinkApps.Stop(Seconds(3600.));

    // I am creating three different route to ISS by creating three different Sockets. Based on the distance sending App choose Socket
    Address sinkAddressArray[3];

    //Creating sink addresses based on the route -> Creating Socket address by binding the port and IP address
    Address sinkAddress(InetSocketAddress(interfaceN1N4.GetAddress(1), sinkPort)); //address of ISS through tdrs9 or n1


    sinkAddressArray[0] = sinkAddress;
    //    std::cout<< sockArr[0] <<std::endl;

    Address sinkAddress1(InetSocketAddress(interfaceN2N4.GetAddress(1), sinkPort)); //address of ISS through tdrs8 or n2
    sinkAddressArray[1] = sinkAddress1;

    Address sinkAddress2(InetSocketAddress(interfaceN3N4.GetAddress(1), sinkPort)); //address of ISS through tdrs10 or n3
    sinkAddressArray[2] = sinkAddress2;

    //</editor-fold>

    //Creating 3 Sockets in the earth station then we will bind these three socket to ISS's 3 sockets
    // we creating these sockets here for hooking the trace sinks 

    Ptr<Socket> *socketArr = new Ptr<Socket>[3];

    for (int i = 0; i < 3; i++) {
        socketArr[i] = Socket::CreateSocket(satNodes.Get(0), TcpSocketFactory::GetTypeId());
    }



    Ptr<SatelliteApp> app = CreateObject<SatelliteApp> ();
    app->Setup(socketArr, sinkAddressArray, 1040, DataRate("586Kbps"), satNodes, groundMng,
            n1tdrs9Mng, n2tdrs8Mng,
            n3tdrs10Mng, issMng);
    satNodes.Get(0)->AddApplication(app);
    app->SetStartTime(Seconds(1.));
    app->SetStopTime(Seconds(65.));

    //<editor-fold defaultstate="collapsed" desc="Trace hooks or connect with trace source with our function">
    //Congestion window trace
    AsciiTraceHelper asciiTraceHelper;
    Ptr<OutputStreamWrapper> stream = asciiTraceHelper.CreateFileStream("sim7_sock1_cwnd.data");
    socketArr[0]->TraceConnectWithoutContext("CongestionWindow", MakeBoundCallback(&CwndChange, stream));

    Ptr<OutputStreamWrapper> stream1 = asciiTraceHelper.CreateFileStream("sim7_sock2_cwnd.data");
    socketArr[1]->TraceConnectWithoutContext("CongestionWindow", MakeBoundCallback(&CwndChange, stream1));

    Ptr<OutputStreamWrapper> stream2 = asciiTraceHelper.CreateFileStream("sim7_sock3_cwnd.data");
    socketArr[2]->TraceConnectWithoutContext("CongestionWindow", MakeBoundCallback(&CwndChange, stream2));

    //Packet drop 
    
    PcapHelper pcapHelper;
    Ptr<PcapFileWrapper> file = pcapHelper.CreateFile("sim7_Iss.pcap", std::ios::out, PcapHelper::DLT_PPP);
    Ptr<PcapFileWrapper> file1 = pcapHelper.CreateFile("sim7_e.pcap", std::ios::out, PcapHelper::DLT_PPP);
    Ptr<PcapFileWrapper> file2 = pcapHelper.CreateFile("sim7_Tdrs1-earth.pcap", std::ios::out, PcapHelper::DLT_PPP);
    Ptr<PcapFileWrapper> file3 = pcapHelper.CreateFile("sim7_Tdrs2-iss.pcap", std::ios::out, PcapHelper::DLT_PPP);

    deviceN1N4.Get(1)->TraceConnectWithoutContext("PhyRxDrop", MakeBoundCallback(&RxDrop, file));
    deviceN2N4.Get(1)->TraceConnectWithoutContext("PhyRxDrop", MakeBoundCallback(&RxDrop, file3));
    deviceN0N1.Get(0)->TraceConnectWithoutContext("PhyRxDrop", MakeBoundCallback(&RxDrop, file1));
    deviceN0N1.Get(1)->TraceConnectWithoutContext("PhyRxDrop", MakeBoundCallback(&RxDrop, file2));
    
    //</editor-fold>
    // and setup ip routing tables to get total ip-level connectivity.
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    Simulator::Stop(Seconds(66.));
    Simulator::Run();
    Simulator::Destroy();

    return 0;
}
