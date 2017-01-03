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
 * MasterUtils.h
 *
 *  Created on: 12 nov. 2016
 *      Author: mikaelr
 */

#ifndef SRC_MASTER_MASTERUTILS_H_
#define SRC_MASTER_MASTERUTILS_H_

#include <functional>
#include <memory>

#include "eventwrapper/EventLoop.h"

class MasterUtils
{
  public:
    MasterUtils();
    ~MasterUtils();
};

/**
 * Helper class. Have one of these as you member.
 * Set up timeout i a certain amount of time.
 * Will Handle disarming of timer in case of destruction.
 */
class Timer
{
  public:
    Timer(EventLoop& loop);

    // Issue a timeout in a number of sec. Vall the given function then.
    void makeTimeout(double timeout, std::function<void()> fkn);

    void makeTimeoutAbs(double timeout, std::function<void()> fkn);

    // cancel an already set timer.
    void cleanTimeout();

  private:
    EventLoop& m_loop;

    std::shared_ptr<TimeoutWatcher> m_tokenTimeout;

    // Helper for delayed callback. Will derive a weak ptr to this
    // that is handed out to e.g. external timers etc.
    // If *this is deleted when callback happens, weak_ptr return nullptr.
    std::shared_ptr<void*> m_timeoutHelper;
};

#endif /* SRC_MASTER_MASTERUTILS_H_ */
