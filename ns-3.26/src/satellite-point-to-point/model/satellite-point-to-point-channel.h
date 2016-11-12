/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2007 University of Washington
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

#ifndef SATELLITE_POINT_TO_POINT_CHANNEL_H
#define SATELLITE_POINT_TO_POINT_CHANNEL_H

#include <list>
#include "ns3/channel.h"
#include "ns3/ptr.h"
#include "ns3/nstime.h"
#include "ns3/data-rate.h"
#include "ns3/traced-callback.h"
#include "ns3/propagation-delay-model.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/random-variable-stream.h"
#include "ns3/log.h"
namespace ns3 {

    class SatellitePointToPointNetDevice;
    class FriisPropagationLossModel;
    class PropagationDelayModel;
    class Packet;

    struct Parameters {
        double rxPowerDbm;
    };

    /**
     * \ingroup pp-satellite
     * \brief Simple Point To Point Satellite Channel.
     *
     * This class represents a very simple point to point satellite channel.  Think full
     * duplex RS-232 or RS-422 with null modem and no handshaking.  There is no
     * multi-drop capability on this channel -- there can be a maximum of two 
     * point-to-point net devices connected.
     *
     * There are two "wires" in the channel.  The first device connected gets the
     * [0] wire to transmit on.  The second device gets the [1] wire.  There is a
     * state (IDLE, TRANSMITTING) associated with each wire.
     *
     * \see Attach
     * \see TransmitStart
     */
    class SatellitePointToPointChannel : public Channel {
    public:
        /**
         * \brief Get the TypeId
         *
         * \return The TypeId for this class
         */
        static TypeId GetTypeId(void);

        /**
         * \brief Create a SatellitePointToPointChannel
         *
         * By default, you get a channel that has an "infinitely" fast 
         * transmission speed and zero delay.
         */
        SatellitePointToPointChannel();

        /**
         * \brief Attach a given netdevice to this channel
         * \param device pointer to the netdevice to attach to the channel
         */
        void Attach(Ptr<SatellitePointToPointNetDevice> device);

        /**
         * \brief Transmit a packet over this channel
         * \param p Packet to transmit
         * \param src Source SatellitePointToPointNetDevice
         * \param txTime Transmit time to apply
         * \returns true if successful (currently always true)
         */
        virtual bool TransmitStart(Ptr<Packet> p, Ptr<SatellitePointToPointNetDevice> src, Time txTime);

        /**
         * \brief Get number of devices on this channel
         * \returns number of devices on this channel
         */
        virtual uint32_t GetNDevices(void) const;

        /**
         * \brief Get SatellitePointToPointNetDevice corresponding to index i on this channel
         * \param i Index number of the device requested
         * \returns Ptr to SatellitePointToPointNetDevice requested
         */
        Ptr<SatellitePointToPointNetDevice> GetSatellitePointToPointDevice(uint32_t i) const;

        /**
         * \brief Get NetDevice corresponding to index i on this channel
         * \param i Index number of the device requested
         * \returns Ptr to NetDevice requested
         */
        virtual Ptr<NetDevice> GetDevice(uint32_t i) const;

        /**
         * \param loss the new propagation loss model.
         */
        void SetPropagationLossModel(Ptr<FriisPropagationLossModel> loss);
        /**
         * \param delay the new propagation delay model.
         */
        void SetPropagationDelayModel(Ptr<PropagationDelayModel> delay);

        /**
         * \brief Get receiver gain corresponding antenna of this channel
         */
        double GetReceiverGain() const;
        /**
         * \param recieverGain of the receiver antenna.
         */
        void SetReceiverGain(double receiverGain);

        /**
         * \brief Get transmitter gain corresponding antenna of this channel
         */
        double GetTransmitterGain() const;
        /**
         * \param Set transmitterGain of the receiver antenna.
         */
        void SetTransmitterGain(double transmitterGain);

        /**
         * \param Get transmitPowerDbm of the transmitter
         */

        double GetTxPowerdBm() const;
        /**
         * \param Set transmitPowerDbm of the transmitter
         */

        void SetTxPowerdBm(double txPowerdBm);
        /**
         * \param Get transmitPowerDbm of the transmitter
         */

        double GetFrequency() const;
        /**
         * \param Set transmitPowerDbm of the transmitter
         */

        void SetFrequency(double frequency);
        

    protected:
        /**
         * \brief Get the delay associated with this channel
         * \returns Time delay
         */
        Time GetDelay(void) const;

        /**
         * \brief Check to make sure the link is initialized
         * \returns true if initialized, asserts otherwise
         */
        bool IsInitialized(void) const;

        /**
         * \brief Get the net-device source 
         * \param i the link requested
         * \returns Ptr to SatellitePointToPointNetDevice source for the
         * specified link
         */
        Ptr<SatellitePointToPointNetDevice> GetSource(uint32_t i) const;

        /**
         * \brief Get the net-device destination
         * \param i the link requested
         * \returns Ptr to SatellitePointToPointNetDevice destination for
         * the specified link
         */
        Ptr<SatellitePointToPointNetDevice> GetDestination(uint32_t i) const;

        /**
         * TracedCallback signature for packet transmission animation events.
         *
         * \param [in] packet The packet being transmitted.
         * \param [in] txDevice the TransmitTing NetDevice.
         * \param [in] rxDevice the Receiving NetDevice.
         * \param [in] duration The amount of time to transmit the packet.
         * \param [in] lastBitTime Last bit receive time (relative to now)
         * \deprecated The non-const \c Ptr<NetDevice> argument is deprecated
         * and will be changed to \c Ptr<const NetDevice> in a future release.
         */
        typedef void (* TxRxAnimationCallback)
        (Ptr<const Packet> packet,
                Ptr<NetDevice> txDevice, Ptr<NetDevice> rxDevice,
                Time duration, Time lastBitTime);

    private:
        /** Each point to point link has exactly two net devices. */
        static const int N_DEVICES = 2;

        Time m_delay; //!< Propagation delay
        int32_t m_nDevices; //!< Devices of this channel
        double m_transmitterGain; //!< Transmitter antenna gain
        double m_receiverGain; //!< Receiver antenna gain
        double m_txPowerdBm; //!< transmit power
        double m_frequency; //!< frequency
       
        double m_lineLoss; //!< Line Loss
        double m_atmosphericAttenuation; //!< La in our case it's 0
        double m_BoltzmanConstindB; //!< 10* log(k)
        double m_ts; //!< Ts = 412.037k ~ 26.1493621646 dB
        
        Ptr<NormalRandomVariable> m_normalRandVar; //!< Normal Random Variable

        /**
         * The trace source for the packet transmission animation events that the 
         * device can fire.
         * Arguments to the callback are the packet, transmitting
         * net device, receiving net device, transmission time and 
         * packet receipt time.
         *
         * \see class CallBackTraceSource
         * \deprecated The non-const \c Ptr<NetDevice> argument is deprecated
         * and will be changed to \c Ptr<const NetDevice> in a future release.
         */
        TracedCallback<Ptr<const Packet>, // Packet being transmitted
                       Ptr<NetDevice>, // Transmitting NetDevice
                       Ptr<NetDevice>, // Receiving NetDevice
                       Time, // Amount of time to transmit the pkt
                       Time // Last bit receive time (relative to now)
                       > m_txrxPointToPoint;

        /** \brief Wire states
         *
         */
        enum ChannelState {
            /** Initializing state */
            INITIALIZING,
            /** Idle state (no transmission from NetDevice) */
            IDLE,
            /** Transmitting state (data being transmitted from NetDevice. */
            TRANSMITTING,
            /** Propagating state (data is being propagated in the channel. */
            PROPAGATING
        };

        /**
         * \brief Wire model for the SatellitePointToPointChannel
         */
        class Link {
        public:

            /** \brief Create the link, it will be in INITIALIZING state
             *
             */
            Link() : m_state(INITIALIZING), m_src(0), m_dst(0) {
            }

            ChannelState m_state; //!< State of the link
            Ptr<SatellitePointToPointNetDevice> m_src; //!< First NetDevice
            Ptr<SatellitePointToPointNetDevice> m_dst; //!< Second NetDevice
        };

        Link m_link[N_DEVICES]; //!< Link model
        Ptr<FriisPropagationLossModel> m_loss; //!< Propagation loss model
        Ptr<PropagationDelayModel> m_propagationDelay; //!< Propagation delay model
    };

} // namespace ns3

#endif /* SATELLITE_POINT_TO_POINT_CHANNEL_H */
