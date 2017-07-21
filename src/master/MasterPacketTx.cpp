/**
 * This file is part of SerialNet.
 *
 *  SerialNet is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SerialNet is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SerialNet.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * MasterPacketTx.cpp
 *
 *  Created on: 25 nov. 2016
 *      Author: mikaelr
 */
#include "utility/Log.h"

#include "MasterPacketTx.h"

void
MasterPacketTx::sendToken(LocalAddress destAddr)
{
    LOG_TRACE << "Send token to: " << int(destAddr);
    packet::GrantToken p(destAddr);
    m_masterTx->sendMasterPacket(packet::fromHeader(p));
}

void
MasterPacketTx::sendMasterStop()
{
    packet::MasterEnded p;
    p.m_type = MessageType::master_ended;
    m_masterTx->sendMasterPacket(packet::fromHeader(p));
}

void
MasterPacketTx::sendMasterStart()
{
    packet::MasterStarted p;
    p.m_type = MessageType::master_started;
    m_masterTx->sendMasterPacket(packet::fromHeader(p));
}

void
MasterPacketTx::sendAddressDiscovery()
{
    packet::AddressDiscovery p;
    p.m_randomDelay = 0;
    p.m_randomHoldOff = 0;
    p.m_type = MessageType::address_discovery;
    m_masterTx->sendMasterPacket(packet::fromHeader(p));
}

void
MasterPacketTx::sendAddressReply(LocalAddress local, packet::UniqueId id)
{
    packet::AddressReply ar;
    ar.m_assigned = local;
    ar.m_type = MessageType::address_reply;
    ar.m_uniqueId = id;
    m_masterTx->sendMasterPacket(packet::fromHeader(ar));
}
