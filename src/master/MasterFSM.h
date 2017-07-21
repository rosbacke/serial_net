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
 * MasterFSM.h
 *
 *  Created on: 13 nov. 2016
 *      Author: mikaelr
 */

#ifndef SRC_MASTER_MASTERFSM_H_
#define SRC_MASTER_MASTERFSM_H_

#include "Action.h"
#include "Event.h"
#include "eventwrapper/EventLoop.h"
#include "utility/Timer.h"
#include "utility/VecQueue.h"
#include "interfaces/TimeServiceIf.h"

#include "statechart/StateChart.h"

class Config;
class MasterPacketIf;
class MasterPacketTx;
class DynamicHandler;
class TimeServiceIf;


class States
{
  public:
    using Event = ::Event;
    enum class StateId
    {
        idle,
        startMaster,
        sendingToken,
        waitClientPacketDone,
        queryAddress,
        waitAddressReply
    };

    static std::string toString(StateId)
    {
        return "";
    }
};

class MasterFSM : public FsmBase<MasterFSM, States>
{

  public:
    MasterFSM() = delete;
    MasterFSM(TimeServiceIf& ts, MasterPacketIf* rx, MasterPacketTx* tx,
              DynamicHandler* dh, Config* config);

    void startAction(const Action& action);

    bool actionActive() const
    {
        return m_currentAction.m_action != Action::Cmd::do_nothing;
    }

    void reportActionResult(Action::ReturnValue rv);

    const Action& currentAction() const
    {
        return m_currentAction;
    }

    struct Remotes
    {
        Remotes(MasterPacketIf* rx, MasterPacketTx* tx, DynamicHandler* dh)
            : m_rx(rx), m_tx(tx), m_dh(dh)
        {
        }

        MasterPacketIf* m_rx;
        MasterPacketTx* m_tx;
        DynamicHandler* m_dh;
    };

    TimeServiceIf& m_ts;
    Remotes m_remotes;
    Config* m_config = nullptr;
    TimeServiceIf::Timer m_timer;

  private:
    Action m_currentAction;
};

#endif /* SRC_MASTER_MASTERFSM_H_ */
