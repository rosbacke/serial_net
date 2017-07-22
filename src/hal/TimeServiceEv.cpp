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

#include "utility/Log.h"
#include <functional>
#include <memory>
#include <utility>

using Timer = TimeServiceIf::Timer;
using TimePoint = TimeServiceIf::TimePoint;

namespace
{

class TimeWatcher : public TimeServiceIf::TimerImpl
{
  public:
    TimeWatcher(EventLoop& evLoop) : m_data(evLoop)
    {
    }

    ~TimeWatcher() override
    {
    }

    void makeTimeout(double timespan, std::function<void()> fkn,
                     std::shared_ptr<TimeServiceIf::TimerImpl>& tw)
    {
        // Note: fkn() might replace the owner of this class. Get a
        // copy of the ptr to ensure it's still alive when we get back.
        m_tw = tw;
        m_data.start(timespan, [fkn, this]() {
            fkn();
            m_tw.reset();
        });
    }

    void cancel() override
    {
        m_data.cancel();
    }

    TimeoutData m_data;

    // Need this to keep TimeoutData object alive in case of early cancellation.
    std::shared_ptr<TimeServiceIf::TimerImpl> m_tw;
};
}

TimeServiceIf::Timer
TimeServiceEv::makeTimeout(double timespan, std::function<void()> fkn)
{
    auto tw = std::make_shared<TimeWatcher>(m_loop);
    std::shared_ptr<TimeServiceIf::TimerImpl> ti = tw;
    tw->makeTimeout(timespan, fkn, ti);
    return TimeServiceIf::Timer(std::move(ti));
}

TimeServiceIf::Timer
TimeServiceEv::makeTimeoutAbs(TimePoint timepoint, std::function<void()> fkn)
{
    auto tw = std::make_shared<TimeWatcher>(m_loop);
    std::shared_ptr<TimeServiceIf::TimerImpl> ti = tw;
    tw->makeTimeout(timepoint - now(), fkn, ti);
    return TimeServiceIf::Timer(std::move(ti));
}
