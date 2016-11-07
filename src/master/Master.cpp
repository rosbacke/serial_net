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
#include "Addresses.h"
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
};

Master::Master(React::Loop& loop, MasterRxIf* mr, MasterTxIf* mt,
               LocalAddress ownClientAddress, Config* cfg)
    : m_loop(loop), m_masterRx(mr), m_masterTx(mt), m_state(initState),
      m_nextState(initState), m_ownClientAddress(ownClientAddress),
      m_addresses(cfg->masterLowAddress(), cfg->masterHighAddress(), loop, cfg)
{
    m_timeoutHelper = std::make_shared<void*>(nullptr);
    if (mr)
    {
        mr->regMasterRx(this);
    }
    emitEvent(EvId::entry);
    postEvent(Event::Id::init);
}

Master::~Master()
{
    if (m_masterRx)
    {
        m_masterRx->regMasterRx(0);
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
        else
        {
            LOG_WARN << "Failed timeout cb.";
        }
        return false;
    });
    LOG_TRACE << "Timeout setup.";
}

void
Master::makeTimeoutAbs(double timeout)
{
    auto weakPtr = std::weak_ptr<void*>(m_timeoutHelper);
    m_tokenTimeout =
        m_loop.onTimeout(timeout - m_loop.now(), [this, weakPtr]() -> bool {
            auto ptr = weakPtr.lock();
            if (ptr)
            {
                tokenTimeout();
            }
            else
            {
                LOG_WARN << "Failed timeoutabs cb.";
            }
            return false;
        });
    LOG_TRACE << "TimeoutAbs setup.";
}

void
Master::cleanTimeout()
{
    LOG_TRACE << "Timeout cleanup.";

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
    LOG_TRACE << "Do evId:" << Event::toString(ev)
              << " in state: " << toString(m_state);
    m_state(*this, ev);
}

void
Master::masterPacketReceived(MessageType type,
                             const MsgEtherIf::EtherPkt& packet)
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
        break;
    case MessageType::mac_update:
        postEvent(EvId::rx_client_packet);
        break;
    }
}

void
Master::tokenTimeout()
{
    LOG_TRACE << "Timeout expired, post event.";
    m_tokenTimeout.reset();
    postEvent(EvId::token_timeout);
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
    case EvId::token_timeout:
    case EvId::entry:
    {
        using State = Addresses::Action::State;
        auto action = me.m_addresses.nextAction();
        LOG_DEBUG << "Next action:" << ::toString(action.m_action)
                  << " addr:" << action.m_address
                  << " time: " << action.m_nextTime;
        switch (action.m_action)
        {
        case State::delay:
            me.makeTimeoutAbs(action.m_nextTime);
            break;
        case State::send_token:
            me.transition(sendNextTokenState);
            break;
        }
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
    case EvId::token_timeout:
    case EvId::entry:
    {
        using State = Addresses::Action::State;
        auto action = me.m_addresses.nextAction();
        switch (action.m_action)
        {
        case State::delay:
            me.makeTimeoutAbs(action.m_nextTime);
            break;
        case State::send_token:
        {
            auto addr = action.m_address;
            if (addr == me.m_ownClientAddress)
            {
                LOG_TRACE << "Give token to own client.";
                if (me.m_masterTx->sendClientPacket(false))
                {
                    // Client is sending.
                    me.transition(sentState);
                }
                else
                {
                    // Client do not have anything to send.
                    me.m_addresses.gotReturnToken();
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
        me.m_addresses.packetStarted();
        me.transition(idleState);
        return true;

    case EvId::rx_return_token:
        // Client got nothing to send. Move on.
        me.m_addresses.gotReturnToken();
        me.transition(idleState);
        return true;

    case EvId::token_timeout:
        if (me.m_masterRx->packetRxInProgress())
        {
            me.makeTimeout(me.m_config->masterTokenClientTimeout());
        }
        else
        {
            // Timeout. Assume the client isn't there. move on.
            me.m_addresses.tokenTimeout();
            me.transition(idleState);
        }
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
