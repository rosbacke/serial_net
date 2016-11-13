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
 * MasterFSM.cpp
 *
 *  Created on: 13 nov. 2016
 *      Author: mikaelr
 */

#include "MasterFSM.h"

#include "Master.h"
#include "utility/Config.h"
#include "utility/Log.h"
#include <gsl/gsl>

#define EL(x) {MasterFSM::x, #x},

using namespace std::string_literals;
using namespace gsl;

std::vector<MasterFSM::State> MasterFSM::m_states = {
    //
    EL(initState)                 //
    EL(idleState)                 //
    EL(sendingTokenState)         //
    EL(waitClientPacketDoneState) //
};

std::string
MasterFSM::toString(StateFkn fkn)
{
    auto iter = std::find_if(
        m_states.begin(), m_states.end(),
        [&](const MasterFSM::State& el) { return fkn == el.m_fkn; });
    return iter != m_states.end() ? iter->m_str : "";
}

MasterFSM::MasterFSM(Master* master, Config* config, React::Loop& loop)
    : m_state(initState), m_nextState(nullptr), m_master(master),
      m_config(config), m_timer(loop){};

void
MasterFSM::start()
{
    Event entry(EvId::entry);
    emitEvent(entry);
    postEvent(Event::Id::init);
}

void
MasterFSM::postEvent(const Event& event)
{
    bool doProcess = m_events.empty();
    if (doProcess)
    {
        stateProcess(event); // We know the queue is empty here.
        while (!m_events.empty())
        {
            auto& ev = m_events.front();
            stateProcess(ev);
            m_events.pop();
        }
    }
    else
    {
        m_events.push(event);
    }
}

void
MasterFSM::stateProcess(const Event& ev)
{
    emitEvent(ev);
    while (m_nextState)
    {
        emitEvent(Event(EvId::exit));
        m_state = m_nextState;
        m_nextState = nullptr;
        emitEvent(Event(EvId::entry));
    }
}

void
MasterFSM::emitEvent(const Event& ev)
{
    LOG_TRACE << "Do evId:" << Event::toString(ev.m_id)
              << " in state: " << toString(m_state);
    m_state(*this, ev);
}

void
MasterFSM::startTimer(double interval)
{
    m_timer.makeTimeout(interval, [&]() {
        LOG_TRACE << "Timeout expired, post event.";
        postEvent(EvId::timer_timeout);
    });
}

void
MasterFSM::startTimerAbs(double timepoint)
{
    m_timer.makeTimeoutAbs(timepoint, [&]() {
        LOG_TRACE << "Timepoint passed, post event.";
        postEvent(EvId::timer_timeout);
    });
}

bool
MasterFSM::initState(MasterFSM& me, const Event& event)
{
    auto ev = event.m_id;
    switch (ev)
    {
    case EvId::entry:
        me.m_master->sendMasterStartStop(false);
        break;
    case EvId::exit:
        break;
    case EvId::init:
        me.transition(idleState);
        return true;
    default:
        break;
    }
    return false;
}

bool
MasterFSM::idleState(MasterFSM& me, const Event& event)
{
    auto ev = event.m_id;
    switch (ev)
    {
    case EvId::timer_timeout:
    case EvId::entry:
    {
        using Cmd = Action::Cmd;
        auto action = me.m_master->m_addresses.nextAction();
        LOG_DEBUG << "Next action:" << ::toString(action.m_action)
                  << " addr:" << action.m_address
                  << " time: " << action.m_nextTime;
        switch (action.m_action)
        {
        case Cmd::delay:
            me.startTimerAbs(action.m_nextTime);
            break;
        case Cmd::send_token:
        {
            auto addr = action.m_address;
            if (addr == me.m_master->m_ownClientAddress)
            {
                LOG_TRACE << "Give token to own client.";
                if (me.m_master->m_masterTx->txQueueEmpty())
                {
                    // Client do not have anything to send.
                    me.m_master->m_addresses.gotReturnToken();
                    me.transition(idleState);
                }
                else
                {
                    // Client is sending.
                    me.m_master->m_masterTx->sendClientPacket();
                    me.transition(waitClientPacketDoneState);
                }
            }
            else
            {
                // Send token to some other client.
                me.m_master->sendToken(addr);
                me.transition(sendingTokenState);
            }
            break;
        }
        case Cmd::query_address:
            me.transition(queryAddressState);
            break;
        }
        break;
    }
    case EvId::exit:
        me.m_timer.cleanTimeout();
        break;
    default:
        break;
    }
    return false;
}

bool
MasterFSM::sendingTokenState(MasterFSM& me, const Event& event)
{
    auto ev = event.m_id;
    switch (ev)
    {
    case EvId::entry:
        me.startTimer(me.m_config->masterTokenGrantTx2RxTime());
        break;

    case EvId::exit:
        me.m_timer.cleanTimeout();
        break;

    case EvId::rx_grant_token:
        me.transition(waitClientPacketDoneState);
        return true;

    case EvId::timer_timeout:
        // Timeout. We should have received the grant token by now.
        // Something went wrong.
        LOG_WARN << "Did not detect my own transmission of the token.";
        me.transition(idleState);
        return true;

    default:
        break;
    }
    return false;
}

bool
MasterFSM::waitClientPacketDoneState(MasterFSM& me, const Event& event)
{
    auto ev = event.m_id;
    switch (ev)
    {
    case EvId::entry:
        me.startTimer(me.m_config->masterTokenClientTimeout());
        break;

    case EvId::exit:
        me.m_timer.cleanTimeout();
        break;

    case EvId::rx_client_packet:
        me.m_master->m_addresses.packetStarted();
        me.transition(idleState);
        return true;

    case EvId::rx_return_token:
        // Client got nothing to send. Move on.
        me.m_master->m_addresses.gotReturnToken();
        me.transition(idleState);
        return true;

    case EvId::timer_timeout:
        if (me.m_master->m_masterRx->packetRxInProgress())
        {
            me.startTimer(me.m_config->masterTokenClientTimeout());
        }
        else
        {
            // Timeout. Assume the client isn't there. move on.
            me.m_master->m_addresses.tokenTimeout();
            me.transition(idleState);
        }
        return true;

    default:
        break;
    }
    return false;
}

bool
MasterFSM::queryAddressState(MasterFSM& me, const Event& event)
{
    auto ev = event.m_id;
    switch (ev)
    {
    case EvId::entry:
        LOG_WARN << "Address query not implemented.";
        me.m_master->m_addresses.addressQueryDone();
        me.transition(idleState);
        break;
    default:
        return true;
    };
    return false;
};
