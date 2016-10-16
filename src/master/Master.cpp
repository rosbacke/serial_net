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
#include "core/PacketTypeCodec.h"
#include "utility/Config.h"
#include "utility/Log.h"
#include <algorithm>
#include <array>

#define EL(x) {Master::x, #x},

using namespace std::string_literals;
using namespace gsl;

std::vector<Master::State> Master::m_states = {
    //
    EL(initState)          //
    EL(idleState)          //
    EL(sendNextTokenState) //
    EL(sendingState)       //
    EL(sentState)          //
    EL(receivingState)     //
};

Master::Master(React::Loop& loop, MasterChannelIf* ps, int ownClientAddress,
               Config* cfg)
    : m_loop(loop), m_masterChannel(ps), m_nextToken(0), m_state(initState),
      m_nextState(initState), m_ownClientAddress(ownClientAddress),
      m_timeoutHelper(std::make_shared<void*>(nullptr)), m_config(cfg)
{
    if (ps)
    {
        ps->regMasterRx(this);
    }
    m_nextToken = m_config->masterLowAddress();
    emitEvent(EvId::entry);
    postEvent(Event::Id::init);
}

Master::~Master()
{
    if (m_masterChannel)
    {
        m_masterChannel->regMasterRx(0);
    }
}

void
Master::makeTimeout(double timeout)
{
    auto weakPtr = std::weak_ptr<void*>(m_timeoutHelper);
    m_tokenTimeout = m_loop.onTimeout(timeout, [this, weakPtr]() -> bool {
        auto ptr = weakPtr.lock();
        if (ptr)
        {
            tokenTimeout();
        }
        return false;
    });
}

void
Master::cleanTimeout()
{
    if (m_tokenTimeout)
    {
        m_tokenTimeout->cancel();
        m_tokenTimeout.reset();
    }
}

void
Master::exitMaster()
{
    sendMasterStartStop(true);
}

void
Master::postEvent(EvId event)
{
    bool doProcess = m_events.empty();
    if (doProcess)
    {
        stateProcess(event); // We know the queue is empty here.
        while (!m_events.empty())
        {
            EvId ev = m_events.front();
            stateProcess(ev);
            m_events.pop_front();
        }
    }
    else
    {
        m_events.push_back(event);
    }
}

void
Master::stateProcess(const EvId& ev)
{
    emitEvent(ev);
    while (m_state != m_nextState)
    {
        emitEvent(EvId::exit);
        m_state = m_nextState;
        emitEvent(EvId::entry);
    }
}

void
Master::emitEvent(const EvId& ev)
{
    // LOG() << "Do evId:" << Event::toString(ev) << " in state: " <<
    // toString(m_state);
    m_state(*this, ev);
}

void
Master::masterPacketReceived(MessageType type, const ByteVec& packet)
{
    switch (type)
    {
    case MessageType::grant_token:
        postEvent(EvId::rx_grant_token);
        break;

    case MessageType::return_token:
        postEvent(EvId::rx_return_token);
        break;

    case MessageType::send_packet:
        postEvent(EvId::rx_client_packet);
        break;

    case MessageType::master_started:
    case MessageType::master_ended:
    case MessageType::send_tun_packet:
    case MessageType::lookup_address:
    case MessageType::address_data:
    	break;
    case MessageType::mac_update:
        postEvent(EvId::rx_client_packet);
        break;
    }
}

void
Master::tokenTimeout()
{
    m_tokenTimeout.reset();
    postEvent(EvId::token_timeout);
    // finishedPrevToken();
}

void
Master::sendToken(int destAddr)
{
    LOG_TRACE << "Send token to: " << destAddr;
    ByteVec packet;
    packet.resize(2);
    packet[0] = to_byte(static_cast<uint8_t>(MessageType::grant_token));
    packet[1] = to_byte(static_cast<uint8_t>(destAddr));
    m_masterChannel->sendMasterPacket(packet);
}

void
Master::sendMasterStartStop(bool stop)
{
    LOG_INFO << "Send master "s + (stop ? "stop"s : "start"s);
    ByteVec packet;
    packet.resize(1);

    packet[0] = to_byte(static_cast<uint8_t>(
        stop ? MessageType::master_ended : MessageType::master_started));
    m_masterChannel->sendMasterPacket(packet);
}

bool
Master::initState(Master& me, const EvId& ev)
{
    switch (ev)
    {
    case EvId::entry:
        me.sendMasterStartStop(false);
        break;
    case EvId::exit:
        break;
    case EvId::init:
        me.transition(sendNextTokenState);
        return true;
    default:
        break;
    }
    return false;
}

bool
Master::idleState(Master& me, const EvId& ev)
{
    switch (ev)
    {
    case EvId::entry:
    {
        if (me.m_nextToken == me.m_config->masterLowAddress() &&
            !me.m_anyPacketSent)
        {
            me.makeTimeout(me.m_config->masterTokenCycleTime());
        }
        else
        {
            me.m_anyPacketSent = false;
            me.transition(sendNextTokenState);
        }
        break;
    }
    case EvId::token_timeout:
    {
        me.transition(sendNextTokenState);
        break;
    }
    case EvId::exit:
        me.cleanTimeout();
        break;
    default:
        break;
    }
    return false;
}

bool
Master::sendNextTokenState(Master& me, const EvId& ev)
{
    switch (ev)
    {
    case EvId::entry:
    {
        int addr = me.m_nextToken++;
        if (me.m_nextToken > me.m_config->masterHighAddress())
        {
            me.m_nextToken = me.m_config->masterLowAddress();
        }
        if (addr == me.m_ownClientAddress)
        {
            LOG_TRACE << "Give token to own client.";
            if (me.m_masterChannel->sendClientPacket())
            {
                // Client is sending.
                me.transition(sentState);
            }
            else
            {
                // Client do not have anything to send.
                me.transition(idleState);
            }
        }
        else
        {
            // Send token to some other client.
            me.sendToken(addr);
            me.transition(sendingState);
        }
        break;
    }
    default:
        return false;
    }
    return false;
}

bool
Master::sendingState(Master& me, const EvId& ev)
{
    switch (ev)
    {
    case EvId::entry:
        me.makeTimeout(me.m_config->masterTokenGrantTx2RxTime());
        break;

    case EvId::exit:
        me.cleanTimeout();
        break;

    case EvId::rx_grant_token:
        me.transition(sentState);
        return true;
#if 0
	case EvId::token_timeout:
		// Timeout. We should have received the grant token by now.
		// Something went wrong.
		me.transition(idleState);
		return true;
#endif
    default:
        break;
    }
    return false;
}

bool
Master::sentState(Master& me, const EvId& ev)
{
    switch (ev)
    {
    case EvId::entry:
        me.makeTimeout(me.m_config->masterTokenClientTimeout());
        break;

    case EvId::exit:
        me.cleanTimeout();
        break;

    case EvId::rx_client_packet:
        me.m_anyPacketSent = true;
        me.transition(idleState);
        return true;

    case EvId::rx_return_token:
        // Client got nothing to send. Move on.
        me.transition(idleState);
        return true;

    case EvId::token_timeout:
        if (me.m_masterChannel->packetRxInProgress())
        {
            me.makeTimeout(me.m_config->masterTokenClientTimeout());
        }
        else
        {
            // Timeout. Assume the client isn't there. move on.
            me.transition(idleState);
        }
        return true;

    default:
        break;
    }
    return false;
}

bool
Master::receivingState(Master& me, const EvId& ev)
{
    switch (ev)
    {
    case EvId::entry:
        break;
    case EvId::exit:
        break;
    case EvId::rx_grant_token:
        me.transition(idleState);
        return true;
    default:
        break;
    }
    return false;
}

std::string
Master::toString(StateFkn fkn)
{
    auto iter =
        std::find_if(m_states.begin(), m_states.end(),
                     [&](const Master::State& el) { return fkn == el.m_fkn; });
    return iter != m_states.end() ? iter->m_str : "";
}

std::string
Master::Event::toString(Id id)
{
#define CASE(x) \
    case Id::x: \
        return #x

    switch (id)
    {
        CASE(entry);
        CASE(exit);
        CASE(init);
        CASE(rx_grant_token);
        CASE(rx_client_packet);
        CASE(rx_return_token);
        CASE(token_timeout);
    }
    return "";
}
