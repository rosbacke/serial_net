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

#include "Event.h"
#include "MasterUtils.h"
#include "utility/VecQueue.h"

class Master;
class Config;

class MasterFSM
{
  public:
    MasterFSM(Master* master, Config* config, React::Loop& loop);
    ~MasterFSM(){};

    using EvId = Event::Id;
    typedef bool (*StateFkn)(MasterFSM& self, const Event& ev);

    void postEvent(EvId id)
    {
        Event ev(id);
        postEvent(ev);
    }

    void postEvent(const Event& event);

    static std::string toString(StateFkn fkn);

    void start();

  private:
    void emitEvent(const Event& ev);

    void startTimer(double interval);
    void startTimerAbs(double timepoint);

    struct State
    {
        StateFkn m_fkn;
        const char* m_str;
    };

    static std::vector<State> m_states;

    // Startup state.
    static bool initState(MasterFSM& me, const Event& ev);

    // Master has the token. No client is allowed on the line.
    // Using the scheduler to determine the next action.
    static bool idleState(MasterFSM& me, const Event& ev);

    // Token is being transmitted to client.
    static bool sendingTokenState(MasterFSM& me, const Event& ev);

    // Token is sent. Waiting for client to start and finish its packet.
    static bool waitClientPacketDoneState(MasterFSM& me, const Event& ev);

    // Send out a discovery message to find if any unit needs an address.
    static bool queryAddressState(MasterFSM& me, const Event& event);

    void transition(StateFkn fkn)
    {
        m_nextState = fkn;
    }

    void stateProcess(const Event& ev);

    StateFkn m_state;
    StateFkn m_nextState;
    VecQueue<Event> m_events;

    Master* m_master;
    Config* m_config;
    Timer m_timer;
};

#endif /* SRC_MASTER_MASTERFSM_H_ */
