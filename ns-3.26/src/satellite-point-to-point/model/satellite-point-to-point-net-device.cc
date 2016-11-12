/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007, 2008 University of Washington
 *
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

#include "satellite-point-to-point-net-device.h"

#include "sppp-header.h"
#include "satellite-point-to-point-channel.h"
#include "ns3/log.h"
#include "ns3/queue.h"
#include "ns3/simulator.h"
#include "ns3/mac48-address.h"
#include "ns3/llc-snap-header.h"
#include "ns3/error-model.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/uinteger.h"
#include "ns3/pointer.h"
#include "ns3/double.h"
namespace ns3 {

    NS_LOG_COMPONENT_DEFINE("SatellitePointToPointNetDevice");

    NS_OBJECT_ENSURE_REGISTERED(SatellitePointToPointNetDevice);

    TypeId SatellitePointToPointNetDevice::GetTypeId(void) {
        static TypeId tid =
                TypeId("ns3::SatellitePointToPointNetDevice").SetParent<NetDevice>().SetGroupName(
                "SatellitePointToPoint").AddConstructor<
                SatellitePointToPointNetDevice>().AddAttribute("Mtu",
                "The MAC-level Maximum Transmission Unit",
                UintegerValue(DEFAULT_MTU),
                MakeUintegerAccessor(
                &SatellitePointToPointNetDevice::SetMtu,
                &SatellitePointToPointNetDevice::GetMtu),
                MakeUintegerChecker<uint16_t>()).AddAttribute("Address",
                "The MAC address of this device.",
                Mac48AddressValue(Mac48Address("ff:ff:ff:ff:ff:ff")),
                MakeMac48AddressAccessor(
                &SatellitePointToPointNetDevice::m_address),
                MakeMac48AddressChecker()).AddAttribute("DataRate",
                "The default data rate for point to point links",
                DataRateValue(DataRate("32768b/s")),
                MakeDataRateAccessor(
                &SatellitePointToPointNetDevice::m_bps),
                MakeDataRateChecker()).AddAttribute("ReceiveErrorModel",
                "The receiver error model used to simulate packet loss",
                PointerValue(),
                MakePointerAccessor(
                &SatellitePointToPointNetDevice::m_receiveErrorModel),
                MakePointerChecker<ErrorModel>()).AddAttribute(
                "InterframeGap",
                "The time to wait between packet (frame) transmissions",
                TimeValue(Seconds(0.0)),
                MakeTimeAccessor(
                &SatellitePointToPointNetDevice::m_tInterframeGap),
                MakeTimeChecker())

                //
                // Transmit queueing discipline for the device which includes its own set
                // of trace hooks.
                //
                .AddAttribute("TxQueue",
                "A queue to use as the transmit queue in the device.",
                PointerValue(),
                MakePointerAccessor(
                &SatellitePointToPointNetDevice::m_queue),
                MakePointerChecker<Queue>())

                //
                // Trace sources at the "top" of the net device, where packets transition
                // to/from higher layers.
                //
                .AddTraceSource("MacTx",
                "Trace source indicating a packet has arrived "
                "for transmission by this device",
                MakeTraceSourceAccessor(
                &SatellitePointToPointNetDevice::m_macTxTrace),
                "ns3::Packet::TracedCallback").AddTraceSource("MacTxDrop",
                "Trace source indicating a packet has been dropped "
                "by the device before transmission",
                MakeTraceSourceAccessor(
                &SatellitePointToPointNetDevice::m_macTxDropTrace),
                "ns3::Packet::TracedCallback").AddTraceSource(
                "MacPromiscRx",
                "A packet has been received by this device, "
                "has been passed up from the physical layer "
                "and is being forwarded up the local protocol stack.  "
                "This is a promiscuous trace,",
                MakeTraceSourceAccessor(
                &SatellitePointToPointNetDevice::m_macPromiscRxTrace),
                "ns3::Packet::TracedCallback").AddTraceSource("MacRx",
                "A packet has been received by this device, "
                "has been passed up from the physical layer "
                "and is being forwarded up the local protocol stack.  "
                "This is a non-promiscuous trace,",
                MakeTraceSourceAccessor(
                &SatellitePointToPointNetDevice::m_macRxTrace),
                "ns3::Packet::TracedCallback")
#if 0
                // Not currently implemented for this device
                .AddTraceSource("MacRxDrop",
                "Trace source indicating a packet was dropped "
                "before being forwarded up the stack",
                MakeTraceSourceAccessor(&SatellitePointToPointNetDevice::m_macRxDropTrace),
                "ns3::Packet::TracedCallback")
#endif
                //
                // Trace souces at the "bottom" of the net device, where packets transition
                // to/from the channel.
                //
                .AddTraceSource("PhyTxBegin",
                "Trace source indicating a packet has begun "
                "transmitting over the channel",
                MakeTraceSourceAccessor(
                &SatellitePointToPointNetDevice::m_phyTxBeginTrace),
                "ns3::Packet::TracedCallback").AddTraceSource("PhyTxEnd",
                "Trace source indicating a packet has been "
                "completely transmitted over the channel",
                MakeTraceSourceAccessor(
                &SatellitePointToPointNetDevice::m_phyTxEndTrace),
                "ns3::Packet::TracedCallback").AddTraceSource("PhyTxDrop",
                "Trace source indicating a packet has been "
                "dropped by the device during transmission",
                MakeTraceSourceAccessor(
                &SatellitePointToPointNetDevice::m_phyTxDropTrace),
                "ns3::Packet::TracedCallback")
#if 0
                // Not currently implemented for this device
                .AddTraceSource("PhyRxBegin",
                "Trace source indicating a packet has begun "
                "being received by the device",
                MakeTraceSourceAccessor(&SatellitePointToPointNetDevice::m_phyRxBeginTrace),
                "ns3::Packet::TracedCallback")
#endif
                .AddTraceSource("PhyRxEnd",
                "Trace source indicating a packet has been "
                "completely received by the device",
                MakeTraceSourceAccessor(
                &SatellitePointToPointNetDevice::m_phyRxEndTrace),
                "ns3::Packet::TracedCallback").AddTraceSource("PhyRxDrop",
                "Trace source indicating a packet has been "
                "dropped by the device during reception",
                MakeTraceSourceAccessor(
                &SatellitePointToPointNetDevice::m_phyRxDropTrace),
                "ns3::Packet::TracedCallback")

                //
                // Trace sources designed to simulate a packet sniffer facility (tcpdump).
                // Note that there is really no difference between promiscuous and
                // non-promiscuous traces in a point-to-point link.
                //
                .AddTraceSource("Sniffer",
                "Trace source simulating a non-promiscuous packet sniffer "
                "attached to the device",
                MakeTraceSourceAccessor(
                &SatellitePointToPointNetDevice::m_snifferTrace),
                "ns3::Packet::TracedCallback").AddTraceSource(
                "PromiscSniffer",
                "Trace source simulating a promiscuous packet sniffer "
                "attached to the device",
                MakeTraceSourceAccessor(
                &SatellitePointToPointNetDevice::m_promiscSnifferTrace),
                "ns3::Packet::TracedCallback")
                .AddAttribute("MdsValue",
                "The minimum detectable signal value (dBm) at the receiver antenna",
                DoubleValue(-82.5),
                MakeDoubleAccessor(&SatellitePointToPointNetDevice::m_mdsValue),
                MakeDoubleChecker<double> ())
                ;
        return tid;
    }

    SatellitePointToPointNetDevice::SatellitePointToPointNetDevice() :
    m_txMachineState(READY), m_channel(0), m_linkUp(false), m_currentPkt(0) {
        NS_LOG_FUNCTION(this);
    }

    SatellitePointToPointNetDevice::~SatellitePointToPointNetDevice() {
        NS_LOG_FUNCTION(this);
    }

    void SatellitePointToPointNetDevice::AddHeader(Ptr<Packet> p,
            uint16_t protocolNumber) {
        NS_LOG_FUNCTION(this << p << protocolNumber);
        SpppHeader sppp;
        sppp.SetProtocol(EtherToPpp(protocolNumber));
        p->AddHeader(sppp);
    }

    bool SatellitePointToPointNetDevice::ProcessHeader(Ptr<Packet> p,
            uint16_t& param) {
        NS_LOG_FUNCTION(this << p << param);
        SpppHeader ppp;
        p->RemoveHeader(ppp);
        param = PppToEther(ppp.GetProtocol());
        return true;
    }

    void SatellitePointToPointNetDevice::DoInitialize(void) {
        NS_LOG_FUNCTION(this);
        // The traffic control layer, if installed, has aggregated a
        // NetDeviceQueueInterface object to this device
        m_queueInterface = GetObject<NetDeviceQueueInterface>();
        NetDevice::DoInitialize();
    }

    void SatellitePointToPointNetDevice::DoDispose() {
        NS_LOG_FUNCTION(this);
        m_node = 0;
        m_channel = 0;
        m_receiveErrorModel = 0;
        m_currentPkt = 0;
        m_queue = 0;
        m_queueInterface = 0;
        NetDevice::DoDispose();
    }

    bool SatellitePointToPointNetDevice::TransmitStart(Ptr<Packet> p) {
        NS_LOG_FUNCTION(this << p);
        NS_LOG_LOGIC("UID is " << p->GetUid() << ")");

        //
        // This function is called to start the process of transmitting a packet.
        // We need to tell the channel that we've started wiggling the wire and
        // schedule an event that will be executed when the transmission is complete.
        //
        NS_ASSERT_MSG(m_txMachineState == READY, "Must be READY to transmit");
        m_txMachineState = BUSY;
        m_currentPkt = p;
        m_phyTxBeginTrace(m_currentPkt);

        Time txTime = m_bps.CalculateBytesTxTime(p->GetSize());
        Time txCompleteTime = txTime + m_tInterframeGap;

        NS_LOG_LOGIC(
                "Schedule TransmitCompleteEvent in " << txCompleteTime.GetSeconds() << "sec");
        Simulator::Schedule(txCompleteTime,
                &SatellitePointToPointNetDevice::TransmitComplete, this);

        /*my change*/
        bool result = m_physical->TransmitToChannel(p, this, m_channel, txTime);
        //	bool result = m_channel->TransmitStart(p, this, txTime);

        if (result == false) {
            m_phyTxDropTrace(p);
        }
        return result;
    }

    void SatellitePointToPointNetDevice::TransmitComplete(void) {
        NS_LOG_FUNCTION(this);

        //
        // This function is called to when we're all done transmitting a packet.
        // We try and pull another packet off of the transmit queue.  If the queue
        // is empty, we are done, otherwise we need to start transmitting the
        // next packet.
        //
        NS_ASSERT_MSG(m_txMachineState == BUSY, "Must be BUSY if transmitting");
        m_txMachineState = READY;

        NS_ASSERT_MSG(m_currentPkt != 0,
                "SatellitePointToPointNetDevice::TransmitComplete(): m_currentPkt zero");

        m_phyTxEndTrace(m_currentPkt);
        m_currentPkt = 0;

        Ptr<NetDeviceQueue> txq;
        if (m_queueInterface) {
            txq = m_queueInterface->GetTxQueue(0);
        }

        Ptr<QueueItem> item = m_queue->Dequeue();
        if (item == 0) {
            NS_LOG_LOGIC("No pending packets in device queue after tx complete");
            if (txq) {
                txq->Wake();
            }
            return;
        }

        //
        // Got another packet off of the queue, so start the transmit process again.
        // If the queue was stopped, start it again. Note that we cannot wake the upper
        // layers because otherwise a packet is sent to the device while the machine
        // state is busy, thus causing the assert in TransmitStart to fail.
        //
        if (txq && txq->IsStopped()) {
            txq->Start();
        }
        Ptr<Packet> p = item->GetPacket();
        m_snifferTrace(p);
        m_promiscSnifferTrace(p);
        TransmitStart(p);
    }

    bool SatellitePointToPointNetDevice::Attach(
            Ptr<SatellitePointToPointChannel> ch) {
        NS_LOG_FUNCTION(this << &ch);

        m_channel = ch;

        m_channel->Attach(this);

        //
        // This device is up whenever it is attached to a channel.  A better plan
        // would be to have the link come up when both devices are attached, but this
        // is not done for now.
        //
        NotifyLinkUp();
        return true;
    }

    void SatellitePointToPointNetDevice::SetQueue(Ptr<Queue> q) {
        NS_LOG_FUNCTION(this << q);
        m_queue = q;
    }

    void SatellitePointToPointNetDevice::SetReceiveErrorModel(Ptr<ErrorModel> em) {
        NS_LOG_FUNCTION(this << em);
        m_receiveErrorModel = em;
    }

    void SatellitePointToPointNetDevice::ReceivePkt(Ptr<Packet> packet, double rxPower) {
        NS_LOG_FUNCTION(this << packet);
        uint16_t protocol = 0;
        if (rxPower < m_mdsValue) {
//            std::cout << "Power is less than threshold " << rxPower << std::endl;

            //
            // If we have an error model and it indicates that it is time to lose a
            // corrupted packet, don't forward this packet up, let it go.
            //
            m_phyRxDropTrace(packet);
            
        } else if (m_receiveErrorModel && m_receiveErrorModel->IsCorrupt(packet)) {
            //
            // If we have an error model and it indicates that it is time to lose a
            // corrupted packet, don't forward this packet up, let it go.
            //
            m_phyRxDropTrace(packet);
        } else {
            //
            // Hit the trace hooks.  All of these hooks are in the same place in this
            // device because it is so simple, but this is not usually the case in
            // more complicated devices.
            //
            m_snifferTrace(packet);
            m_promiscSnifferTrace(packet);
            m_phyRxEndTrace(packet);

            //
            // Trace sinks will expect complete packets, not packets without some of the
            // headers.
            //
            Ptr<Packet> originalPacket = packet->Copy();

            //
            // Strip off the point-to-point protocol header and forward this packet
            // up the protocol stack.  Since this is a simple point-to-point link,
            // there is no difference in what the promisc callback sees and what the
            // normal receive callback sees.
            //
            ProcessHeader(packet, protocol);

            if (!m_promiscCallback.IsNull()) {
                m_macPromiscRxTrace(originalPacket);
                m_promiscCallback(this, packet, protocol, GetRemote(), GetAddress(),
                        NetDevice::PACKET_HOST);
            }

            m_macRxTrace(originalPacket);
            m_rxCallback(this, packet, protocol, GetRemote());
        }
    }

    void SatellitePointToPointNetDevice::Receive(Ptr<Packet> packet) {
        NS_LOG_FUNCTION(this << packet);
        uint16_t protocol = 0;
        if (m_receiveErrorModel && m_receiveErrorModel->IsCorrupt(packet)) {
            //
            // If we have an error model and it indicates that it is time to lose a
            // corrupted packet, don't forward this packet up, let it go.
            //
            m_phyRxDropTrace(packet);
        } else {
            //
            // Hit the trace hooks.  All of these hooks are in the same place in this
            // device because it is so simple, but this is not usually the case in
            // more complicated devices.
            //
            m_snifferTrace(packet);
            m_promiscSnifferTrace(packet);
            m_phyRxEndTrace(packet);

            //
            // Trace sinks will expect complete packets, not packets without some of the
            // headers.
            //
            Ptr<Packet> originalPacket = packet->Copy();

            //
            // Strip off the point-to-point protocol header and forward this packet
            // up the protocol stack.  Since this is a simple point-to-point link,
            // there is no difference in what the promisc callback sees and what the
            // normal receive callback sees.
            //
            ProcessHeader(packet, protocol);

            if (!m_promiscCallback.IsNull()) {
                m_macPromiscRxTrace(originalPacket);
                m_promiscCallback(this, packet, protocol, GetRemote(), GetAddress(),
                        NetDevice::PACKET_HOST);
            }

            m_macRxTrace(originalPacket);
            m_rxCallback(this, packet, protocol, GetRemote());
        }
    }

    //<editor-fold defaultstate="collapsed" desc="getter and setter">

    Ptr<Queue> SatellitePointToPointNetDevice::GetQueue(void) const {
        NS_LOG_FUNCTION(this);
        return m_queue;
    }

    void SatellitePointToPointNetDevice::NotifyLinkUp(void) {
        NS_LOG_FUNCTION(this);
        m_linkUp = true;
        m_linkChangeCallbacks();
    }

    void SatellitePointToPointNetDevice::SetIfIndex(const uint32_t index) {
        NS_LOG_FUNCTION(this);
        m_ifIndex = index;
    }

    uint32_t SatellitePointToPointNetDevice::GetIfIndex(void) const {
        return m_ifIndex;
    }

    Ptr<Channel> SatellitePointToPointNetDevice::GetChannel(void) const {
        return m_channel;
    }

    //
    // This is a point-to-point device, so we really don't need any kind of address
    // information.  However, the base class NetDevice wants us to define the
    // methods to get and set the address.  Rather than be rude and assert, we let
    // clients get and set the address, but simply ignore them.

    void SatellitePointToPointNetDevice::SetAddress(Address address) {
        NS_LOG_FUNCTION(this << address);
        m_address = Mac48Address::ConvertFrom(address);
    }

    Address SatellitePointToPointNetDevice::GetAddress(void) const {
        return m_address;
    }

    bool SatellitePointToPointNetDevice::IsLinkUp(void) const {
        NS_LOG_FUNCTION(this);
        return m_linkUp;
    }

    void SatellitePointToPointNetDevice::AddLinkChangeCallback(
            Callback<void> callback) {
        NS_LOG_FUNCTION(this);
        m_linkChangeCallbacks.ConnectWithoutContext(callback);
    }

    //
    // This is a point-to-point device, so every transmission is a broadcast to
    // all of the devices on the network.
    //

    bool SatellitePointToPointNetDevice::IsBroadcast(void) const {
        NS_LOG_FUNCTION(this);
        return true;
    }

    double SatellitePointToPointNetDevice::GetMdsValue() {
        return m_mdsValue;
    }


    //
    // We don't really need any addressing information since this is a 
    // point-to-point device.  The base class NetDevice wants us to return a
    // broadcast address, so we make up something reasonable.
    //

    Address SatellitePointToPointNetDevice::GetBroadcast(void) const {
        NS_LOG_FUNCTION(this);
        return Mac48Address("ff:ff:ff:ff:ff:ff");
    }

    bool SatellitePointToPointNetDevice::IsMulticast(void) const {
        NS_LOG_FUNCTION(this);
        return true;
    }

    Address SatellitePointToPointNetDevice::GetMulticast(
            Ipv4Address multicastGroup) const {
        NS_LOG_FUNCTION(this);
        return Mac48Address("01:00:5e:00:00:00");
    }

    Address SatellitePointToPointNetDevice::GetMulticast(Ipv6Address addr) const {
        NS_LOG_FUNCTION(this << addr);
        return Mac48Address("33:33:00:00:00:00");
    }

    bool SatellitePointToPointNetDevice::IsPointToPoint(void) const {
        NS_LOG_FUNCTION(this);
        return true;
    }

    bool SatellitePointToPointNetDevice::IsBridge(void) const {
        NS_LOG_FUNCTION(this);
        return false;
    }

    void SatellitePointToPointNetDevice::SetDataRate(DataRate bps) {
        NS_LOG_FUNCTION(this);
        m_bps = bps;
    }
    
    DataRate SatellitePointToPointNetDevice::GetDataRate() {
        NS_LOG_FUNCTION(this);
        return m_bps;
    }

    void SatellitePointToPointNetDevice::SetInterframeGap(Time t) {
        NS_LOG_FUNCTION(this << t.GetSeconds());
        m_tInterframeGap = t;
    }

    // </editor-fold>

    bool SatellitePointToPointNetDevice::Send(Ptr<Packet> packet,
            const Address &dest, uint16_t protocolNumber) {
        Ptr<NetDeviceQueue> txq;
        if (m_queueInterface) {
            txq = m_queueInterface->GetTxQueue(0);
        }

        NS_ASSERT_MSG(!txq || !txq->IsStopped(),
                "Send should not be called when the device is stopped");

        NS_LOG_FUNCTION(this << packet << dest << protocolNumber);
        NS_LOG_LOGIC("p=" << packet << ", dest=" << &dest);
        NS_LOG_LOGIC("UID is " << packet->GetUid());

        //
        // If IsLinkUp() is false it means there is no channel to send any packet
        // over so we just hit the drop trace on the packet and return an error.
        //
        if (IsLinkUp() == false) {
            m_macTxDropTrace(packet);
            return false;
        }

        //
        // Stick a point to point protocol header on the packet in preparation for
        // shoving it out the door.
        //
        AddHeader(packet, protocolNumber);

        m_macTxTrace(packet);

        //
        // We should enqueue and dequeue the packet to hit the tracing hooks.
        //
        if (m_queue->Enqueue(Create<QueueItem>(packet))) {
            //
            // If the channel is ready for transition we send the packet right now
            //
            if (m_txMachineState == READY) {
                packet = m_queue->Dequeue()->GetPacket();
                m_snifferTrace(packet);
                m_promiscSnifferTrace(packet);
                return TransmitStart(packet);
            }
            return true;
        }

        // Enqueue may fail (overflow). Stop the tx queue, so that the upper layers
        // do not send packets until there is room in the queue again.
        m_macTxDropTrace(packet);
        if (txq) {
            txq->Stop();
        }
        return false;
    }
    // <editor-fold defaultstate="collapsed" desc="Getter and setter and other code">

    bool SatellitePointToPointNetDevice::SendFrom(Ptr<Packet> packet,
            const Address &source, const Address &dest, uint16_t protocolNumber) {
        NS_LOG_FUNCTION(this << packet << source << dest << protocolNumber);
        return false;
    }

    Ptr<Node> SatellitePointToPointNetDevice::GetNode(void) const {
        return m_node;
    }

    void SatellitePointToPointNetDevice::SetNode(Ptr<Node> node) {
        NS_LOG_FUNCTION(this);
        m_node = node;
    }

    bool SatellitePointToPointNetDevice::NeedsArp(void) const {
        NS_LOG_FUNCTION(this);
        return false;
    }

    void SatellitePointToPointNetDevice::SetReceiveCallback(
            NetDevice::ReceiveCallback cb) {
        m_rxCallback = cb;
    }

    void SatellitePointToPointNetDevice::SetPromiscReceiveCallback(
            NetDevice::PromiscReceiveCallback cb) {
        m_promiscCallback = cb;
    }

    bool SatellitePointToPointNetDevice::SupportsSendFrom(void) const {
        NS_LOG_FUNCTION(this);
        return false;
    }

    void SatellitePointToPointNetDevice::DoMpiReceive(Ptr<Packet> p) {
        NS_LOG_FUNCTION(this << p);
        Receive(p);
    }

    Address SatellitePointToPointNetDevice::GetRemote(void) const {
        NS_LOG_FUNCTION(this);
        NS_ASSERT(m_channel->GetNDevices() == 2);
        for (uint32_t i = 0; i < m_channel->GetNDevices(); ++i) {
            Ptr<NetDevice> tmp = m_channel->GetDevice(i);
            if (tmp != this) {
                return tmp->GetAddress();
            }
        }
        NS_ASSERT(false);
        // quiet compiler.
        return Address();
    }

    bool SatellitePointToPointNetDevice::SetMtu(uint16_t mtu) {
        NS_LOG_FUNCTION(this << mtu);
        m_mtu = mtu;
        return true;
    }

    uint16_t SatellitePointToPointNetDevice::GetMtu(void) const {
        NS_LOG_FUNCTION(this);
        return m_mtu;
    }

    uint16_t SatellitePointToPointNetDevice::PppToEther(uint16_t proto) {
        NS_LOG_FUNCTION_NOARGS();
        switch (proto) {
            case 0x0021:
                return 0x0800; //IPv4
            case 0x0057:
                return 0x86DD; //IPv6
            default:
                NS_ASSERT_MSG(false, "SPPP Protocol number not defined!");
        }
        return 0;
    }

    uint16_t SatellitePointToPointNetDevice::EtherToPpp(uint16_t proto) {
        NS_LOG_FUNCTION_NOARGS();
        switch (proto) {
            case 0x0800:
                return 0x0021; //IPv4
            case 0x86DD:
                return 0x0057; //IPv6
            default:
                NS_ASSERT_MSG(false, "SPPP Protocol number not defined!");
        }
        return 0;
    }

    Ptr<SatellitePhysical> SatellitePointToPointNetDevice::GetPhysical() {
        return m_physical;
    }

    void SatellitePointToPointNetDevice::SetPhysical(Ptr<SatellitePhysical> obj) {
        m_physical = obj;
    }


    // </editor-fold>
} // namespace ns3
