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
 * TimeServiceEv.cpp
 *
 *  Created on: 18 juli 2017
 *      Author: mikaelr
 */

#include "TimeServiceEv.h"
#include "eventwrapper/EventLoop.h"
#include "utility/Log.h"

using Timer = TimeServiceIf::Timer;
using TimePoint = TimeServiceIf::TimePoint;

namespace
{

class TimeWatcher : public TimeServiceIf::TimerImpl
{
  public:
    TimeWatcher(EventLoop& loop) : m_loop(loop)
    {
        m_timeoutHelper = std::make_shared<void*>(nullptr);
    }

    ~TimeWatcher() override
    {
        if (m_tokenTimeout)
        {
            m_tokenTimeout->cancel();
        }
    }

    void makeTimeout(double timespan, std::function<void()> fkn)
    {
        auto weakPtr = std::weak_ptr<void*>(m_timeoutHelper);
        m_tokenTimeout =
            m_loop.onTimeout(timespan, [this, weakPtr, fkn]() -> bool {
                auto ptr = weakPtr.lock();
                if (ptr)
                {
                    m_tokenTimeout.reset();
                    fkn();
                }
                else
                {
                    LOG_WARN << "Failed timeout cb.";
                }
                return false;
            });
        LOG_TRACE << "Timeout setup.";
    }

    void cancel() override
    {
        LOG_TRACE << "Timeout cleanup.";

        if (m_tokenTimeout)
        {
            m_tokenTimeout->cancel();
            m_tokenTimeout.reset();
        }
    }

    std::shared_ptr<TimeoutWatcher> m_tokenTimeout;

    // Helper for delayed callback. Will derive a weak ptr to this
    // that is handed out to e.g. external timers etc.
    // If *this is deleted when callback happens, weak_ptr return nullptr.
    std::shared_ptr<void*> m_timeoutHelper;

    EventLoop& m_loop;
};
}

TimeServiceIf::Timer
TimeServiceEv::makeTimeout(double timespan, std::function<void()> fkn)
{
    auto tw = std::make_shared<TimeWatcher>(m_loop);
    tw->makeTimeout(timespan, fkn);
    TimeServiceIf::Timer timer(tw);
    return timer;
}

TimeServiceIf::Timer
TimeServiceEv::makeTimeoutAbs(TimePoint timepoint, std::function<void()> fkn)
{
    auto tw = std::make_shared<TimeWatcher>(m_loop);
    tw->makeTimeout(timepoint - now(), fkn);
    TimeServiceIf::Timer timer(tw);
    return timer;
}
