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
 * Addresses.h
 *
 *  Created on: 20 okt. 2016
 *      Author: mikaelr
 */

#ifndef SRC_MASTER_ADDRESSES_H_
#define SRC_MASTER_ADDRESSES_H_

#include "AddressLine.h"
#include "reactcpp.h"
#include "utility/Config.h"

#include <queue>
#include <vector>

/**
 * A table with all the active addresses that the master cares about.
 */
class Addresses
{
  public:
    class Action
    {
      public:
        enum class State
        {
            send_token,
            delay
        };

        Action(State state, int addr, double time)
            : m_action(state), m_address(addr), m_nextTime(time)
        {
        }

        static Action makeDelayAction(double time)
        {
            return Action(State::delay, 0, time);
        }

        static Action makeSendTokenAction(int addr)
        {
            return Action(State::send_token, addr, 0.0);
        }

        State m_action;
        int m_address;
        double m_nextTime;
    };

    /**
     * Set up AddressTable.
     * @param minAddr lowest address to initialize.
     * @param maxAddr highest Address to initialize.
     */
    Addresses(int minAddr, int maxAddr, React::Loop& loop, Config* cfg);
    ~Addresses();

    void gotReturnToken();

    void tokenTimeout();

    // Report that the last sent out token resulted in a packet transmission.
    void packetStarted();

    // Query the next token action to perform.
    Action nextAction();

  private:
    class QueueEl
    {
      public:
        QueueEl() : m_address(0), m_nextTime(0.0)
        {
        }
        QueueEl(int addr, double nextTime)
            : m_address(addr), m_nextTime(nextTime)
        {
        }

        int m_address;
        double m_nextTime;
    };

    friend bool operator>(const QueueEl& lhs, const QueueEl& rhs);

    using Address = int;

    void updateReadyQueue();

    double nextTime(AddressLine::State state);

    AddressLine* find(int address);

    std::vector<AddressLine> m_table;

    int m_minAddr;
    int m_maxAddr;
    React::Loop& m_loop;

    // All active addresses should be in one of these.
    //
    std::priority_queue<QueueEl, std::vector<QueueEl>, std::greater<QueueEl>>
        m_ready;
    bool m_frontInProgress = false;
    Config* m_config;
};

inline bool
operator>(const Addresses::QueueEl& lhs, const Addresses::QueueEl& rhs)
{
    bool greater =
        lhs.m_nextTime > rhs.m_nextTime ||
        (lhs.m_nextTime == rhs.m_nextTime && lhs.m_address > rhs.m_address);
    return greater;
}

std::string
toString(Addresses::Action::State state);

#endif /* SRC_MASTER_ADDRESSES_H_ */
