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

using namespace std::string_literals;
using namespace gsl;

namespace
{

class MasterStateBase : public StateBase<MasterHSM>
{
  protected:
    const Config* m_config = nullptr;
    Master* m_master = nullptr;

    MasterStateBase(MasterHSM& hsm)
        : StateBase(hsm), m_config(hsm.m_config), m_master(hsm.m_master)
    {
    }

    void startTimer(double interval)
    {
        hsm().m_timer.makeTimeout(interval, [&]() {
            LOG_TRACE << "Timeout expired, post event.";
            hsm().postEvent(Event::Id::timer_timeout);
        });
    }

    void startTimerAbs(double timepoint)
    {
        hsm().m_timer.makeTimeoutAbs(timepoint, [&]() {
            LOG_TRACE << "Timepoint passed, post event.";
            hsm().postEvent(Event::Id::timer_timeout);
        });
    }

    void cleanTimer()
    {
        hsm().m_timer.cleanTimeout();
    }
};

class Init : public MasterStateBase
{
  public:
    Init(MasterHSM& fsm) : MasterStateBase(fsm)
    {
        fsm.m_master->m_tx.sendMasterStart();
    }

    bool event(const Event& ev)
    {
        switch (ev.m_id)
        {
        case Event::Id::init:
            transition(StateId::idle);
            return true;
            break;
        default:
            break;
        }
        return false;
    }
};

class Idle : public MasterStateBase
{
  public:
    Idle(MasterHSM& hsm) : MasterStateBase(hsm)
    {
        nextAction();
    }

    ~Idle()
    {
        cleanTimer();
    }

    bool event(const Event& ev)
    {
        switch (ev.m_id)
        {
        case Event::Id::timer_timeout:
            transition(StateId::idle);
            return true;
            break;
        default:
            break;
        }
        return false;
    }

  private:
    void nextAction()
    {
        using Cmd = Action::Cmd;
        auto action = m_master->m_actionHandler.nextAction();
        LOG_DEBUG << "Next action:" << ::toString(action.m_action)
                  << " addr:" << action.m_address
                  << " time: " << action.m_nextTime;
        switch (action.m_action)
        {
        case Cmd::delay:
            startTimerAbs(action.m_nextTime);
            break;
        case Cmd::send_token:
        {
            auto addr = action.m_address;
            if (addr == m_master->m_tx.clientAddress())
            {
                LOG_TRACE << "Give token to own client.";
                if (m_master->m_tx.txQueueEmpty())
                {
                    // Client do not have anything to send.
                    m_master->m_actionHandler.gotReturnToken();
                    transition(StateId::idle);
                }
                else
                {
                    // Client is sending.
                    m_master->m_tx.sendClientPacket();
                    transition(StateId::waitClientPacketDone);
                }
            }
            else
            {
                // Send token to some other client.
                m_master->m_tx.sendToken(addr);
                transition(StateId::sendingToken);
            }
            break;
        }
        case Cmd::query_address:
            transition(StateId::queryAddress);
            break;
        }
    }
};

class SendingToken : public MasterStateBase
{
  public:
    SendingToken(MasterHSM& hsm) : MasterStateBase(hsm)
    {
        startTimer(m_config->masterTokenGrantTx2RxTime());
    }

    ~SendingToken()
    {
        cleanTimer();
    }

    bool event(const Event& event)
    {
        auto ev = event.m_id;
        switch (ev)
        {
        case Event::Id::rx_grant_token:
            transition(StateId::waitClientPacketDone);
            return true;

        case Event::Id::timer_timeout:
            // Timeout. We should have received the grant token by now.
            // Something went wrong.
            LOG_WARN << "Did not detect my own transmission of the token.";
            transition(StateId::idle);
            return true;

        default:
            break;
        }
        return false;
    }
};

class WaitClientPacketDoneState : public MasterStateBase
{
  public:
    WaitClientPacketDoneState(MasterHSM& hsm) : MasterStateBase(hsm)
    {
        startTimer(m_config->masterTokenClientTimeout());
    }

    ~WaitClientPacketDoneState()
    {
        cleanTimer();
    }

    bool event(const Event& event)
    {
        auto ev = event.m_id;
        switch (ev)
        {
        case Event::Id::rx_client_packet:
            m_master->m_actionHandler.packetStarted();
            transition(StateId::idle);
            return true;

        case Event::Id::rx_return_token:
            // Client got nothing to send. Move on.
            m_master->m_actionHandler.gotReturnToken();
            transition(StateId::idle);
            return true;

        case Event::Id::timer_timeout:
            if (m_master->m_masterRx->packetRxInProgress())
            {
                startTimer(m_config->masterTokenClientTimeout());
            }
            else
            {
                // Timeout. Assume the client isn't there. move on.
                m_master->m_actionHandler.tokenTimeout();
                transition(StateId::idle);
            }
            return true;
        default:
            break;
        }
        return false;
    }
};

class QueryAddressState : public MasterStateBase
{
  public:
    QueryAddressState(MasterHSM& hsm) : MasterStateBase(hsm)
    {
        startTimer(m_config->masterTokenClientTimeout());
        m_master->m_tx.sendAddressDiscovery();
    }

    ~QueryAddressState()
    {
        cleanTimer();
    }

    bool event(const Event& event)
    {
        auto ev = event.m_id;
        switch (ev)
        {
        case Event::Id::timer_timeout:
            if (m_master->m_masterRx->packetRxInProgress())
            {
                // Somebody has started to respond. Wait a bit more.
                startTimer(m_config->masterTokenClientTimeout());
                LOG_INFO << "A bit delayed discovery. Waiting more.";
            }
            else
            {
                m_master->m_actionHandler.addressQueryDone();
                transition(StateId::idle);
            }
            return true;

        case Event::Id::rx_address_request:
            m_master->m_dynamicHandler.receivedAddressRequest(
                boost::get<packet::AddressRequest>(event.eventData));

            // Got a request from a client. Process it.
            transition(StateId::waitAddressReply);
            return true;

        default:
            break;
        };
        return false;
    }
};

class WaitAddressReply : public MasterStateBase
{
  public:
    WaitAddressReply(MasterHSM& hsm) : MasterStateBase(hsm)
    {
        startTimer(m_config->masterTokenClientTimeout());
    }

    ~WaitAddressReply()
    {
        cleanTimer();
        m_master->m_actionHandler.addressQueryDone();
    }

    bool event(const Event& event)
    {
        auto ev = event.m_id;
        switch (ev)
        {
        case Event::Id::timer_timeout:
            if (m_master->m_masterRx->packetRxInProgress())
            {
                // Still sending addressReply
                startTimer(m_config->masterTokenClientTimeout());
            }
            else
            {
                transition(StateId::idle);
            }
            return true;

        default:
            break;
        };
        return false;
    }
};

} // anonymous namespace.

MasterHSM::MasterHSM(Master* master, Config* config, EventLoop& loop)
    : m_master(master), m_config(config), m_timer(loop)
{
    addState<Init, StateId::init>();
    addState<Idle, StateId::idle>();
    addState<SendingToken, StateId::sendingToken>();
    addState<WaitClientPacketDoneState, StateId::waitClientPacketDone>();
    addState<QueryAddressState, StateId::queryAddress>();
    addState<WaitAddressReply, StateId::waitAddressReply>();
}
