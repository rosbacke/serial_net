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
 * MasterScheduler.h
 *
 *  Created on: 9 nov. 2016
 *      Author: mikaelr
 */

#ifndef SRC_MASTER_MASTERSCHEDULER_H_
#define SRC_MASTER_MASTERSCHEDULER_H_

#include "Action.h"
#include "eventwrapper/EventLoop.h"
#include "interfaces/SerialProtocol.h"
#include "utility/VecQueue.h"

#include <queue>

class EventLoop;

/**
 * Keep track of upcoming tasks to be done in the future.
 */
class MasterScheduler
{
  public:
    class QueueEl
    {
      public:
        QueueEl(Action action, double nextTime)
            : m_nextTime(nextTime), m_action(action)
        {
        }
        double m_nextTime;
        Action m_action;
    };

    MasterScheduler(){};
    ~MasterScheduler(){};

    void addAction(const Action& a, double t)
    {
        m_waiting.emplace(a, t);
    }

    void addActionNow(const Action& a)
    {
        m_ready.push(a);
    }

    const Action& front() const
    {
        return m_ready.front();
    }

    void pop()
    {
        m_ready.pop();
    }

    bool workToDo(double now)
    {
        while (!m_waiting.empty() && m_waiting.top().m_nextTime <= now)
        {
            m_ready.push(m_waiting.top().m_action);
            m_waiting.pop();
        }
        return !m_ready.empty();
    }

    // Assumes we have already emptied the ready queue. Return time
    // for the first element of the waiting queue.
    double readyTime()
    {
        return m_waiting.empty() ? 0.0 : m_waiting.top().m_nextTime;
    }

  private:
    friend bool operator>(const QueueEl& lhs, const QueueEl& rhs);

    // Actions to be done in the future.
    std::priority_queue<QueueEl, std::vector<QueueEl>, std::greater<QueueEl>>
        m_waiting;

    // Actions ready for processing.
    VecQueue<Action> m_ready;
};

inline bool
operator>(const MasterScheduler::QueueEl& lhs,
          const MasterScheduler::QueueEl& rhs)
{
    bool greater =
        lhs.m_nextTime > rhs.m_nextTime ||
        (lhs.m_nextTime == rhs.m_nextTime && lhs.m_action > rhs.m_action);
    return greater;
}

#endif /* SRC_MASTER_MASTERSCHEDULER_H_ */
