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
 * TimeServiceSim.h
 *
 *  Created on: 20 juli 2017
 *      Author: mikaelr
 */

#ifndef SRC_HAL_TIMESERVICESIM_H_
#define SRC_HAL_TIMESERVICESIM_H_

#include "interfaces/TimeServiceIf.h"

#include <functional>
#include <queue>
#include <vector>

/**
 * Offer a simulated time backbone to the TimeServiceIf.
 */
class TimeServiceSim : public TimeServiceIf
{
  public:
    struct QueueEl
    {
        QueueEl(double time, std::function<void()> fkn)
            : m_time(time), m_fkn(fkn)
        {
        }

        double m_time;
        std::function<void()> m_fkn;
    };

    TimeServiceSim();
    virtual ~TimeServiceSim();

    TimePoint now() const override
    {
        return m_now;
    }

    Timer makeTimeout(double timespan, std::function<void()> fkn) override;

    Timer makeTimeoutAbs(TimePoint timeout, std::function<void()> fkn) override;

    void cancel(Timer& timer) override
    {
    }

    void advance(double delta);

  private:
    friend bool operator>(const QueueEl& lhs, const QueueEl& rhs);

    // Actions to be done in the future.
    std::priority_queue<QueueEl, std::vector<QueueEl>, std::greater<QueueEl>>
        m_waiting;

    double m_now =
        1.0; // Avoid 0.0 since it is often used as 'not a valid time'.
};

inline bool
operator>(const TimeServiceSim::QueueEl& lhs,
          const TimeServiceSim::QueueEl& rhs)
{
    bool greater = lhs.m_time > rhs.m_time;
    return greater;
}

#endif /* SRC_HAL_TIMESERVICESIM_H_ */
