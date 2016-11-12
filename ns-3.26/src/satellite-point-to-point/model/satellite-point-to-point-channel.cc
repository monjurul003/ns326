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

#include "satellite-point-to-point-channel.h"

#include "satellite-point-to-point-net-device.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "satellite-physical.h"
#include "ns3/pointer.h"
#include "ns3/double.h"
namespace ns3 {

    NS_LOG_COMPONENT_DEFINE("SatellitePointToPointChannel");

    NS_OBJECT_ENSURE_REGISTERED(SatellitePointToPointChannel);

    TypeId
    SatellitePointToPointChannel::GetTypeId(void) {
        static TypeId tid = TypeId("ns3::SatellitePointToPointChannel")
                .SetParent<Channel> ()
                .SetGroupName("PointToPoint")
                .AddConstructor<SatellitePointToPointChannel> ()
                .AddAttribute("Delay", "Propagation delay through the channel",
                TimeValue(Seconds(0)),
                MakeTimeAccessor(&SatellitePointToPointChannel::m_delay),
                MakeTimeChecker())
                .AddAttribute("ReceiverGain",
                "The receiver gain (dB) of the receiver antenna",
                DoubleValue(20.0),
                MakeDoubleAccessor(&SatellitePointToPointChannel::SetReceiverGain,
                &SatellitePointToPointChannel::GetReceiverGain),
                MakeDoubleChecker<double> ())
                .AddAttribute("TransmitterGain",
                "The transmitter gain (dB) of the receiver antenna",
                DoubleValue(49.7),
                MakeDoubleAccessor(&SatellitePointToPointChannel::SetTransmitterGain,
                &SatellitePointToPointChannel::GetTransmitterGain),
                MakeDoubleChecker<double> ())
                .AddAttribute("TransmitPowerDbm",
                "The transmit power (dBm) of the transmitter antenna",
                DoubleValue(50.0),
                MakeDoubleAccessor(&SatellitePointToPointChannel::GetTxPowerdBm,
                &SatellitePointToPointChannel::SetTxPowerdBm),
                MakeDoubleChecker<double> ())
                .AddAttribute("Frequency",
                "The frequency of the transmitter antenna",
                DoubleValue(4e9),
                MakeDoubleAccessor(&SatellitePointToPointChannel::GetFrequency,
                &SatellitePointToPointChannel::SetFrequency),
                MakeDoubleChecker<double> ())
                .AddTraceSource("TxRxPointToPoint",
                "Trace source indicating transmission of packet "
                "from the SatellitePointToPointChannel, used by the Animation "
                "interface.",
                MakeTraceSourceAccessor(&SatellitePointToPointChannel::m_txrxPointToPoint),
                "ns3::SatellitePointToPointChannel::TxRxAnimationCallback")
                ;
        return tid;
    }

    //
    // By default, you get a channel that 
    // has an "infitely" fast transmission speed and zero delay.

    SatellitePointToPointChannel::SatellitePointToPointChannel()
    :
    Channel(),
    m_delay(Seconds(0.)),
    m_nDevices(0) {
        NS_LOG_FUNCTION_NOARGS();
        double mean = 0.0;
        double variance = 16.0; //SD = 8dB, SD=10*log(p/p_0) => (p.p_0)=6.31 => (p/p_0)^2 = 39.811 => SD^2 = 16 )
        m_normalRandVar = CreateObject<NormalRandomVariable> ();
        m_normalRandVar->SetAttribute("Mean", DoubleValue(mean));
        m_normalRandVar->SetAttribute("Variance", DoubleValue(variance));
        m_BoltzmanConstindB = -228.59916863; // 10* log(K) == -228.599dB
        m_ts = 26.1493621646; //dB 412.037k ~ 10*log(ts) = 26.1493621646
        m_lineLoss = -0.5; //dB
        m_atmosphericAttenuation = 0.0;
    }

    void
    SatellitePointToPointChannel::Attach(Ptr<SatellitePointToPointNetDevice> device) {
        NS_LOG_FUNCTION(this << device);
        NS_ASSERT_MSG(m_nDevices < N_DEVICES, "Only two devices permitted");
        NS_ASSERT(device != 0);

        m_link[m_nDevices++].m_src = device;
        //
        // If we have both devices connected to the channel, then finish introducing
        // the two halves and set the links to IDLE.
        //
        if (m_nDevices == N_DEVICES) {
            m_link[0].m_dst = m_link[1].m_src;
            m_link[1].m_dst = m_link[0].m_src;
            m_link[0].m_state = IDLE;
            m_link[1].m_state = IDLE;
        }
    }

    bool
    SatellitePointToPointChannel::TransmitStart(
            Ptr<Packet> p,
            Ptr<SatellitePointToPointNetDevice> src,
            Time txTime) {
        NS_LOG_FUNCTION(this << p << src);
        NS_LOG_LOGIC("UID is " << p->GetUid() << ")");

        NS_ASSERT(m_link[0].m_state != INITIALIZING);
        NS_ASSERT(m_link[1].m_state != INITIALIZING);

        uint32_t channel = src == m_link[0].m_src ? 0 : 1;
        /*  My modification starts here*/
        /*
         Pr (Eb/No) (dB) = (Pt + Ls) + Ll + Gt + Gr + La(0dB) - 10 log(k) -10 log(Ts) - 10 log(R) - 5dB + GaussianFading
         * 
         * Ts = 412.037 K
         * K = Boltzman Const.
         * La - atmospheric attenuation we choose 0db
         * 5 db = Implementation loss (-2) + Polarization Loss(-3)
         * Ll = Line loss typically -0.5 dB 
         */

        uint64_t bps = src->GetDataRate().GetBitRate();
        Ptr<MobilityModel> srcMob = src->GetNode()->GetObject<MobilityModel> ();
        Ptr<MobilityModel> destMob = m_link[channel].m_dst->GetNode()->GetObject<MobilityModel>();
        Ptr<FriisPropagationLossModel> frisModel = this->m_loss;
        double rxPower = 1.0;
        if (frisModel != NULL) {
            frisModel->SetFrequency(4e9);
            //    std::cout << " Freq-> "<< frisModel->GetFrequency()<< std::endl;
            rxPower = frisModel->CalcRxPower(m_txPowerdBm, srcMob, destMob) + m_lineLoss - m_ts
                    + m_receiverGain + m_transmitterGain - m_BoltzmanConstindB + m_normalRandVar->GetValue()
                    - (10 * log(bps)) - 5;
//            std::cout << "Sent 50dBm but received -->" << rxPower << std::endl;
        } else {
            NS_LOG_INFO("Friis model has not been initialized.");
        }
        /*end of ajaira code*/
        Parameters param;
        param.rxPowerDbm = rxPower;

        //calculate delay from the position of the node then set 
        m_delay = this->m_propagationDelay->GetDelay(srcMob, destMob);
        //          std::cout << "delay--> "<<m_delay.GetSeconds()<< "s"<< std::endl;


        //    std::cout<<m_link[channel].m_dst->GetAddress()<<std::endl;
        //  Simulator::ScheduleWithContext (m_link[channel].m_dst->GetNode ()->GetId (),
        //                                  txTime + m_delay, &SatellitePointToPointNetDevice::Receive,
        //                                  m_link[channel].m_dst, p);


        Simulator::ScheduleWithContext(m_link[channel].m_dst->GetNode()->GetId(),
                txTime + m_delay, &SatellitePhysical::ReceiveFromChannel,
                m_link[channel].m_dst->GetPhysical(), p, param);

        // Call the tx anim callback on the net device
        m_txrxPointToPoint(p, src, m_link[channel].m_dst, txTime, txTime + m_delay);
        return true;
    }



    // <editor-fold defaultstate="collapsed" desc="Generated Code">

    void SatellitePointToPointChannel::SetPropagationLossModel(Ptr<FriisPropagationLossModel> loss) {
        m_loss = loss;
    }

    void SatellitePointToPointChannel::SetPropagationDelayModel(Ptr<PropagationDelayModel> delay) {
        m_propagationDelay = delay;
    }

    uint32_t
    SatellitePointToPointChannel::GetNDevices(void) const {
        NS_LOG_FUNCTION_NOARGS();
        return m_nDevices;
    }

    Ptr<SatellitePointToPointNetDevice>
    SatellitePointToPointChannel::GetSatellitePointToPointDevice(uint32_t i) const {
        NS_LOG_FUNCTION_NOARGS();
        NS_ASSERT(i < 2);
        return m_link[i].m_src;
    }

    Ptr<NetDevice>
    SatellitePointToPointChannel::GetDevice(uint32_t i) const {
        NS_LOG_FUNCTION_NOARGS();
        return GetSatellitePointToPointDevice(i);
    }

    Time
    SatellitePointToPointChannel::GetDelay(void) const {
        return m_delay;
    }

    Ptr<SatellitePointToPointNetDevice>
    SatellitePointToPointChannel::GetSource(uint32_t i) const {
        return m_link[i].m_src;
    }

    Ptr<SatellitePointToPointNetDevice>
    SatellitePointToPointChannel::GetDestination(uint32_t i) const {
        return m_link[i].m_dst;
    }

    bool
    SatellitePointToPointChannel::IsInitialized(void) const {
        NS_ASSERT(m_link[0].m_state != INITIALIZING);
        NS_ASSERT(m_link[1].m_state != INITIALIZING);
        return true;
    }

    /**
     * \brief Get receiver gain corresponding antenna of this channel
     */
    double SatellitePointToPointChannel::GetReceiverGain() const {
        return m_receiverGain;
    }

    /**
     * \param recieverGain of the receiver antenna.
     */
    void SatellitePointToPointChannel::SetReceiverGain(double receiverGain) {
        m_receiverGain = receiverGain;
    }

    /**
     * \brief Get transmitter gain corresponding antenna of this channel
     */
    double SatellitePointToPointChannel::GetTransmitterGain() const {
        return m_transmitterGain;
    }

    /**
     * \param Set transmitterGain of the receiver antenna.
     */
    void SatellitePointToPointChannel::SetTransmitterGain(double transmitterGain) {
        m_transmitterGain = transmitterGain;
    }

    /**
     * \param Get transmitPowerDbm of the transmitter
     */

    double SatellitePointToPointChannel::GetTxPowerdBm() const {
        return m_txPowerdBm;
    }

    /**
     * \param Set transmitPowerDbm of the transmitter
     */

    void SatellitePointToPointChannel::SetTxPowerdBm(double txPowerdBm) {
        m_txPowerdBm = txPowerdBm;
    }

    double SatellitePointToPointChannel::GetFrequency() const {
        return m_frequency;
    }

    void SatellitePointToPointChannel::SetFrequency(double frequency) {
        m_frequency = frequency;
    }


    // </editor-fold>
} // namespace ns3
