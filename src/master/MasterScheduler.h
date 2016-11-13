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
#include "interfaces/SerialProtocol.h"

#include <queue>

/**
 * Control what the master is supposed to do at certain timepoints.
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

    MasterScheduler();
    ~MasterScheduler();

    void addAction(const Action& a, double t)
    {
        m_ready.emplace(a, t);
    }
    const QueueEl& active() const
    {
        return m_ready.top();
    }
    void pop()
    {
        m_ready.pop();
    }

  private:
    friend bool operator>(const QueueEl& lhs, const QueueEl& rhs);

    // All active addresses should be in one of these.
    std::priority_queue<QueueEl, std::vector<QueueEl>, std::greater<QueueEl>>
        m_ready;
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
