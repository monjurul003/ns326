/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/satellite-point-to-point-module.h"
#include "ns3/applications-module.h"

#include "ns3/mobility-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("FirstScriptExample");

void
PrintLocations(NodeContainer nodes, std::string header) {
    std::cout << header << std::endl;
    for (NodeContainer::Iterator iNode = nodes.Begin(); iNode != nodes.End(); ++iNode) {
        Ptr<Node> object = *iNode;
        Ptr<MobilityModel> position = object->GetObject<MobilityModel> ();
        NS_ASSERT(position != 0);
        Vector pos = position->GetPosition();
        std::cout << "(" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
    }
    std::cout << std::endl;
}

int
main(int argc, char *argv[]) {

    Time::SetResolution(Time::NS);
    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

    NodeContainer nodes;
    nodes.Create(2);

    SatellitePointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("240ms"));
    pointToPoint.SetChannelAttribute("ReceiverGain", DoubleValue(49.7));
    pointToPoint.SetChannelAttribute("TransmitterGain", DoubleValue(20.0));

    NetDeviceContainer devices;
    devices = pointToPoint.Install(nodes);

    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");

    Ipv4InterfaceContainer interfaces = address.Assign(devices);
    /* this is my modification*/
    MobilityHelper mobility;

    mobility.SetPositionAllocator("ns3::GridPositionAllocator",
            "MinX", DoubleValue(0.0),
            "MinY", DoubleValue(0.0),
            "DeltaX", DoubleValue(35475000.0),
            "DeltaY", DoubleValue(10000.0),
            "GridWidth", UintegerValue(3),
            "LayoutType", StringValue("RowFirst"));

    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);

    //    
    //    Ptr<Node> tmpNode = nodes.Get(0);
    //    Vector posOfSat;
    //    posOfSat.x = 50.0;
    //    posOfSat.y = 2.0;
    //    posOfSat.z = 5.0;
    //    Ptr<MobilityModel> mob = tmpNode->GetObject<MobilityModel>();
    //    mob->SetPosition(posOfSat);
    //    int dist = mobility.GetDistanceSquaredBetween(nodes.Get(0), nodes.Get(1));
    //
    //    std::cout << "distance between two nodes: " << std::sqrt(dist) << std::endl;
    //    PrintLocations(nodes, "Test");

    UdpEchoServerHelper echoServer(9);

    ApplicationContainer serverApps = echoServer.Install(nodes.Get(1));
    serverApps.Start(Seconds(1.0));
    serverApps.Stop(Seconds(100.0));

    UdpEchoClientHelper echoClient(interfaces.GetAddress(1), 9);
    echoClient.SetAttribute("MaxPackets", UintegerValue(1));
    echoClient.SetAttribute("Interval", TimeValue(Seconds(1.0)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApps = echoClient.Install(nodes.Get(0));
    clientApps.Start(Seconds(2.0));
    clientApps.Stop(Seconds(100.0));


//    AnimationInterface anim("anim1.xml");
//    anim.SetConstantPosition(nodes.Get(0), 1.0, 2.0);
//    anim.SetConstantPosition(nodes.Get(1), 2.0, 3.0);


    Simulator::Run();
    Simulator::Destroy();
    return 0;
}
