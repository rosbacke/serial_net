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
 * TimeServiceIf.h
 *
 *  Created on: 18 juli 2017
 *      Author: mikaelr
 */

#ifndef SRC_INTERFACES_TIMESERVICEIF_H_
#define SRC_INTERFACES_TIMESERVICEIF_H_

#include <functional>
#include <memory>

class TimeServiceIf
{
  public:
    class TimerImpl
    {
      public:
        virtual void cancel() = 0;

        virtual ~TimerImpl()
        {
        }
    };

    class Timer
    {
      public:
        Timer() : m_timer(nullptr)
        {
        }

        Timer(std::shared_ptr<TimerImpl>&& timer) : m_timer(timer)
        {
        }

        Timer(const Timer& timer) : m_timer(timer.m_timer)
        {
        }
        Timer& operator=(Timer&& t)
        {
            std::swap(t.m_timer, m_timer);
            t.m_timer.reset();
            return *this;
        }
        Timer& operator=(const Timer& t)
        {
            Timer tt(t);
            std::swap(tt.m_timer, m_timer);
            return *this;
        }

        void swap(Timer& t)
        {
            std::swap(m_timer, t.m_timer);
        }

        ~Timer()
        {
            if (m_timer)
            {
                m_timer->cancel();
            }
        }

        void cancel()
        {
            if (m_timer)
            {
                m_timer->cancel();
                m_timer.reset();
            }
        }

        std::shared_ptr<TimerImpl> m_timer;
    };

    using TimePoint = double;

    TimeServiceIf(){};
    virtual ~TimeServiceIf()
    {
    }

    virtual TimePoint now() const = 0;

    virtual Timer makeTimeout(double timespan, std::function<void()> fkn) = 0;

    virtual Timer makeTimeoutAbs(TimePoint timeout,
                                 std::function<void()> fkn) = 0;

    virtual void cancel(Timer& timer) = 0;
};

#endif /* SRC_INTERFACES_TIMESERVICEIF_H_ */
