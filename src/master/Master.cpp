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
 * Master.cpp
 *
 *  Created on: 6 jul 2016
 *      Author: mikaelr
 */

#include "Master.h"
#include "ActionHandler.h"
#include "core/PacketTypeCodec.h"
#include "utility/Config.h"
#include "utility/Log.h"
#include <algorithm>
#include <array>

using namespace std::string_literals;
using namespace gsl;

Master::Master(React::Loop& loop, MasterRxIf* mr, MasterTxIf* mt,
               LocalAddress ownClientAddress, Config* cfg)
    : m_loop(loop), m_masterRx(mr), m_masterTx(mt),
      m_ownClientAddress(ownClientAddress), m_config(cfg),
      m_addresses(loop, cfg), m_fsm(this, cfg, loop)
{
    if (mr)
    {
        mr->regMasterRx(this);
    }
    m_fsm.start();
}

Master::~Master()
{
    if (m_masterRx)
    {
        m_masterRx->regMasterRx(nullptr);
    }
}

void
Master::exitMaster()
{
    sendMasterStartStop(true);
}

void
Master::masterPacketReceived(MessageType type,
                             const MsgEtherIf::EtherPkt& packet)
{
    switch (type)
    {
    case MessageType::grant_token:
        m_fsm.postEvent(EvId::rx_grant_token);
        break;

    case MessageType::return_token:
        m_fsm.postEvent(EvId::rx_return_token);
        break;

    case MessageType::send_packet:
        m_fsm.postEvent(EvId::rx_client_packet);
        break;

    case MessageType::master_started:
    case MessageType::master_ended:
        break;
    case MessageType::mac_update:
        m_fsm.postEvent(EvId::rx_client_packet);
        break;
    }
}

void
Master::sendToken(LocalAddress destAddr)
{
    LOG_TRACE << "Send token to: " << int(destAddr);
    ByteVec packet;
    packet.resize(2);
    packet::GrantToken* p =
        reinterpret_cast<packet::GrantToken*>(packet.data());
    p->m_type = MessageType::grant_token;
    p->m_tokenReceiver = destAddr;
    m_masterTx->sendMasterPacket(packet);
}

void
Master::sendMasterStartStop(bool stop)
{
    LOG_INFO << "Send master "s + (stop ? "stop"s : "start"s);

    ByteVec packet;
    if (stop)
    {
        packet.resize(sizeof(packet::MasterEnded));
        auto* p = packet::toHeader<packet::MasterEnded>(packet.data());
        p->m_type = MessageType::master_ended;
    }
    else
    {
        packet.resize(sizeof(packet::MasterStarted));
        auto* p = packet::toHeader<packet::MasterStarted>(packet.data());
        p->m_type = MessageType::master_started;
    }
    m_masterTx->sendMasterPacket(packet);
}
