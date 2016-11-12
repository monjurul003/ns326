/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016 University of Manitoba
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
 */

/* 
 * File:   satellite-physical.h
 * Author: MIK
 *
 * Created on July 14, 2016, 11:54 AM
 */

#ifndef SATELLITE_PHYSICAL_H
#define	SATELLITE_PHYSICAL_H

#include "ns3/mobility-model.h"
#include "ns3/nstime.h"
#include "ns3/object.h"
#include "satellite-point-to-point-channel.h"
#include "ns3/random-variable-stream.h"
namespace ns3 {

    class SatellitePointToPointChannel;
    class SatellitePointToPointNetDevice;
    class Packet;
    class SatellitePhysical : public Object {
    
    public: 
        static TypeId GetTypeId(void);
        SatellitePhysical();
     
        
        void test(void);
        
        void SetMobility(Ptr<MobilityModel> obj);  
        Ptr<MobilityModel> GetMobility(void);
        
        void SetNetDevice(Ptr<SatellitePointToPointNetDevice> obj);  
        Ptr<SatellitePointToPointNetDevice> GetNetDevice(void);
        
        void SetChannel(Ptr<SatellitePointToPointChannel> obj);  
        Ptr<SatellitePointToPointChannel> GetChannel(void);
        
        bool TransmitToChannel(Ptr<Packet> p, Ptr<SatellitePointToPointNetDevice> src,Ptr<SatellitePointToPointChannel> channel, Time txTime);
        
        void ReceiveFromChannel(Ptr<Packet> p, Parameters param);
        
    private: 
        Ptr<MobilityModel> m_mobility;
        Ptr<SatellitePointToPointNetDevice> m_device;
        Ptr<SatellitePointToPointChannel> m_channel;
        Ptr<UniformRandomVariable> m_random;  //!< Provides uniform random variables.
    };

} //end of namespace ns3

#endif	/* SATELLITE_PHYSICAL_H */

