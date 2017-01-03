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

Master::Master(EventLoop& loop, MasterPacketIf* mr, MasterTxIf* mt, Config* cfg)
    : m_loop(loop), m_masterRx(mr), m_config(cfg), m_tx(mt),
      m_dynamicHandler(loop, cfg, &m_scheduler, &m_tx),
      m_actionHandler(loop, cfg, m_scheduler, &m_dynamicHandler),
      m_fsm(this, cfg, loop)
{
    if (mr)
    {
        mr->regMasterRx(this);
    }
    m_dynamicHandler.setActionHandler(&m_actionHandler);

    m_fsm.setStartState(States::StateId::init);
    m_fsm.postEvent(Event::Id::init);
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
    m_tx.sendMasterStop();
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

    case MessageType::address_discovery:
        break;

    case MessageType::address_request:
        if (packet.size() >=
            static_cast<ptrdiff_t>(sizeof(packet::AddressRequest)))
        {
            auto aReq = packet::toHeader<packet::AddressRequest>(packet);
            Event ev(Event::Id::rx_address_request);
            ev.eventData = *aReq;
            m_fsm.postEvent(ev);
        }
        break;

    case MessageType::address_reply:
        break;
    }
}
