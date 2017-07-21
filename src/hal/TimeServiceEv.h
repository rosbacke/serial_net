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
 * TimeServiceEv.h
 *
 *  Created on: 18 juli 2017
 *      Author: mikaelr
 */

#ifndef SRC_HAL_TIMESERVICEEV_H_
#define SRC_HAL_TIMESERVICEEV_H_

#include <functional>

#include "eventwrapper/EventLoop.h"
#include "interfaces/TimeServiceIf.h"

class EventLoop;

class TimeServiceEv : public TimeServiceIf
{
  public:
    TimeServiceEv(EventLoop& loop) : m_loop(loop)
    {
    }

    ~TimeServiceEv() override{};

    TimePoint now() const override
    {
        return m_loop.now();
    }

    Timer makeTimeout(double timespan, std::function<void()> fkn) override;

    Timer makeTimeoutAbs(TimePoint timeout, std::function<void()> fkn) override;

    void cancel(Timer& timer) override
    {
        timer.cancel();
    }

  private:
    EventLoop& m_loop;
};

#endif /* SRC_HAL_TIMESERVICEEV_H_ */
