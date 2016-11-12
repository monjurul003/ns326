
#include "ns3/log.h"
#include "satellite-physical.h"
#include "satellite-point-to-point-net-device.h"
#include "satellite-point-to-point-channel.h"

#include "ns3/pointer.h"

namespace ns3 {

    NS_LOG_COMPONENT_DEFINE("SatellitePhysical");
    NS_OBJECT_ENSURE_REGISTERED(SatellitePhysical);

    TypeId SatellitePhysical::GetTypeId() {
        static TypeId tid = TypeId("ns3::SatellitePhysical")
                .SetParent<Object>()
                .SetGroupName("SatellitePointToPoint")
                .AddConstructor<SatellitePhysical>()
                ;
        return tid;
    }

    SatellitePhysical::SatellitePhysical() {
        m_random = CreateObject<UniformRandomVariable> ();
    }

    Ptr<SatellitePointToPointChannel> SatellitePhysical::GetChannel() {
        return m_channel;
    }

    void SatellitePhysical::SetChannel(Ptr<SatellitePointToPointChannel> obj) {
        m_channel = obj;
    }

    void SatellitePhysical::test() {
        if (m_mobility == NULL) {
            if (m_channel != NULL) {

                std::cout << "channel set hoise" << std::endl;
            } else {
                std::cout << "channel set hoi nai" << std::endl;
            }
            if (m_device != NULL) {
                std::cout << "ok device set hoise" << std::endl;
                Ptr<MobilityModel> mob = m_device->GetNode()->GetObject<MobilityModel> ();
                std::cout << "asche " << mob->GetPosition().x << std::endl;
                if (mob == 0) {
                    std::cout << "pai nai" << std::endl;
                }
            } else {
                std::cout << "device set hoi nai" << std::endl;
            }

            return;
        } else {
            std::cout << "jhakaknaka" << std::endl;
        }
    }

    Ptr<MobilityModel> SatellitePhysical::GetMobility() {
        if (m_mobility != 0) {
            return m_mobility;
        } else {
            return m_device->GetNode()->GetObject<MobilityModel> ();
        }
    }

    void SatellitePhysical::SetMobility(Ptr<MobilityModel> obj) {
        m_mobility = obj;
    }

    Ptr<SatellitePointToPointNetDevice> SatellitePhysical::GetNetDevice() {
        return m_device;
    }

    void SatellitePhysical::SetNetDevice(Ptr<SatellitePointToPointNetDevice> obj) {
        m_device = obj;
    }

    bool SatellitePhysical::TransmitToChannel(Ptr<Packet> p, Ptr<SatellitePointToPointNetDevice> src, Ptr<SatellitePointToPointChannel> channel, Time txTime) {
        bool result = channel->TransmitStart(p, src, txTime);
        return result;
    }

    void SatellitePhysical::ReceiveFromChannel(Ptr<Packet> p, Parameters param) {
//        double rxPower = m_random->GetValue() * param.rxPowerDbm;
//        std::cout<<"rxPower -->"<<param.rxPowerDbm<<std::endl;
        m_device->ReceivePkt(p, param.rxPowerDbm);
//        m_device->Receive(p);
    }

}
