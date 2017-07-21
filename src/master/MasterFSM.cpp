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

class MasterStateBase : public StateBase<MasterFSM>
{
  protected:
    const Config* m_config = nullptr;

    MasterStateBase(StateArgs args) : StateBase(args), m_config(fsm().m_config)
    {
    }

    void startTimer(double interval)
    {
        fsm().m_timer = fsm().m_ts.makeTimeout(interval, [&]() {
            LOG_TRACE << "Timeout expired, post event.";
            fsm().postEvent(Event::Id::timer_timeout);
        });
    }

    void startTimerAbs(double timepoint)
    {
        fsm().m_timer = fsm().m_ts.makeTimeoutAbs(timepoint, [&]() {
            LOG_TRACE << "Timepoint passed, post event.";
            fsm().postEvent(Event::Id::timer_timeout);
        });
    }

    void cleanTimer()
    {
        fsm().m_timer.cancel();
    }

    const MasterFSM::Remotes& call()
    {
        return fsm().m_remotes;
    }
};

class Idle : public MasterStateBase
{
  public:
    Idle(StateArgs args) : MasterStateBase(args)
    {
        // LOG_DEBUG << "Enter Idle.";
    }

    ~Idle()
    {
        // LOG_DEBUG << "Leaving Idle.";
        // cleanTimer();
    }

    bool event(const Event& ev)
    {
        switch (ev.m_id)
        {
        case Event::Id::check_new_command:
            // LOG_DEBUG << "Idle: check_new_command.";
            decodeAction(fsm().currentAction());
            return true;
            break;

        default:
            break;
        }
        return false;
    }

  private:
    void decodeAction(const Action& action)
    {
        using Cmd = Action::Cmd;

        {
                        LOG_DEBUG << "Next action:" <<
                        Action::toString(action.m_action)
                                  << " addr:" << action.m_address;
        }

        switch (action.m_action)
        {
        case Cmd::do_nothing:
            break;

        case Cmd::send_token:
        {
            auto addr = action.m_address;
            if (addr == call().m_tx->clientAddress())
            {
                // LOG_TRACE << "Give token to own client.";
                if (call().m_tx->txQueueEmpty())
                {
                    // Client do not have anything to send.
                    fsm().reportActionResult(
                        Action::ReturnValue::rx_token_no_packet);
                }
                else
                {
                    // Client is sending.
                    call().m_tx->sendClientPacket();
                    transition(StateId::waitClientPacketDone);
                }
            }
            else
            {
                // Send token to some other client.
                call().m_tx->sendToken(addr);
                transition(StateId::sendingToken);
            }
            break;
        }
        case Cmd::query_address:
            transition(StateId::queryAddress);
            break;

        case Cmd::send_master_start:
            transition(StateId::startMaster);
            break;
        }
    }
};

class MasterStart : public MasterStateBase
{
  public:
    MasterStart(StateArgs args) : MasterStateBase(args)
    {
        call().m_tx->sendMasterStart();
        startTimer(0.1);
    }

    ~MasterStart()
    {
        cleanTimer();
    }

    bool event(const Event& event)
    {
        auto ev = event.m_id;
        switch (ev)
        {
        case Event::Id::rx_pkt_master_start_stop:
            fsm().reportActionResult(Action::ReturnValue::ok);
            transition(StateId::idle);
            return true;

        case Event::Id::timer_timeout:
            // Timeout. We should have received the master start packet
            // by now.
            LOG_WARN << "Did not detect my own transmission of start master.";
            fsm().reportActionResult(Action::ReturnValue::timeout);
            transition(StateId::idle);
            return true;

        default:
            break;
        }
        return false;
    }
};

class SendingToken : public MasterStateBase
{
  public:
    SendingToken(StateArgs args) : MasterStateBase(args)
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
        case Event::Id::rx_pkt_grant_token:
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
    WaitClientPacketDoneState(StateArgs args) : MasterStateBase(args)
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
        case Event::Id::rx_pkt_client_packet:
            fsm().reportActionResult(
                Action::ReturnValue::client_packet_started);
            transition(StateId::idle);
            return true;

        case Event::Id::rx_return_token:
            // Client got nothing to send. Move on.
            fsm().reportActionResult(Action::ReturnValue::rx_token_no_packet);
            transition(StateId::idle);
            return true;

        case Event::Id::timer_timeout:
            if (call().m_rx->packetRxInProgress())
            {
                startTimer(m_config->masterTokenClientTimeout());
            }
            else
            {
                // Timeout. Assume the client isn't there. move on.
                fsm().reportActionResult(Action::ReturnValue::token_timeout);
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
    QueryAddressState(StateArgs args) : MasterStateBase(args)
    {
        startTimer(m_config->masterTokenClientTimeout());
        call().m_tx->sendAddressDiscovery();
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
            if (call().m_rx->packetRxInProgress())
            {
                // Somebody has started to respond. Wait a bit more.
                startTimer(m_config->masterTokenClientTimeout());
                LOG_INFO << "A bit delayed discovery. Waiting more.";
            }
            else
            {
                fsm().reportActionResult(
                    Action::ReturnValue::address_query_done);
                transition(StateId::idle);
            }
            return true;

        case Event::Id::rx_pkt_address_request:
            call().m_dh->receivedAddressRequest(
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
    WaitAddressReply(StateArgs args) : MasterStateBase(args)
    {
        startTimer(m_config->masterTokenClientTimeout());
    }

    ~WaitAddressReply()
    {
        cleanTimer();
        fsm().reportActionResult(Action::ReturnValue::address_query_done);
    }

    bool event(const Event& event)
    {
        auto ev = event.m_id;
        switch (ev)
        {
        case Event::Id::timer_timeout:
            if (call().m_rx->packetRxInProgress())
            {
                // Still sending addressReply
                startTimer(m_config->masterTokenClientTimeout());
            }
            else
            {
                // Report back in destructor.
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

MasterFSM::MasterFSM(TimeServiceIf& ts, MasterPacketIf* rx, MasterPacketTx* tx,
                     DynamicHandler* dh, Config* config)
    : m_ts(ts), m_remotes(rx, tx, dh), m_config(config), m_timer(), m_currentAction()
{
    addState<Idle, StateId::idle>();
    addState<MasterStart, StateId::startMaster>();
    addState<SendingToken, StateId::sendingToken>();
    addState<WaitClientPacketDoneState, StateId::waitClientPacketDone>();
    addState<QueryAddressState, StateId::queryAddress>();
    addState<WaitAddressReply, StateId::waitAddressReply>();
}

void
MasterFSM::startAction(const Action& action)
{
    assert(!actionActive());
    // LOG_DEBUG << "MasterFSM::startAction " <<
    // Action::toString(action.m_action);
    m_currentAction = action;
    postEvent(Event::Id::check_new_command);
};

void
MasterFSM::reportActionResult(Action::ReturnValue rv)
{
    // LOG_DEBUG << "ReportActionResult FSM :" << Action::toString(rv);
    auto cb = m_currentAction.m_reportCB;
    m_currentAction = Action::makeDoNothingAction();
    if (cb)
    {
        cb(rv);
    }
}
