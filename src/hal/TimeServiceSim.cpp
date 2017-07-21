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
 * TimeServiceSim.cpp
 *
 *  Created on: 20 juli 2017
 *      Author: mikaelr
 */

#include "TimeServiceSim.h"


using Timer = TimeServiceIf::Timer;


namespace {

class TimeWatcher : public TimeServiceIf::TimerImpl
{
#if 0
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
#endif
};

// }






class TimerSim : public TimeServiceIf::TimerImpl
{

};
}

TimeServiceSim::TimeServiceSim()
{
	// TODO Auto-generated constructor stub

}

TimeServiceSim::~TimeServiceSim()
{
	// TODO Auto-generated destructor stub
}


Timer
TimeServiceSim::makeTimeout(double timespan, std::function<void()> fkn)
{
	return Timer();
}
