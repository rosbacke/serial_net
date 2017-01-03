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
 * TimeoutWatcher.h
 *
 *  Created on: 3 jan. 2017
 *      Author: mikaelr
 */

#ifndef SRC_EVENTWRAPPER_TIMEOUTWATCHER_H_
#define SRC_EVENTWRAPPER_TIMEOUTWATCHER_H_

#include <ev.h>

#include <memory>

class TimeoutWatcher : ev_timer
{
  public:
    static std::shared_ptr<TimeoutWatcher>
    mkTimeoutWatch(double delay, std::function<bool()> fkn,
                   struct ev_loop* loop)
    {
        auto t = std::make_shared<TimeoutWatcher>(fkn, loop);
        t->setup(delay, t);
        return t;
    }

    ~TimeoutWatcher(){};

    TimeoutWatcher(std::function<bool()> fkn, struct ev_loop* loop)
        : m_fkn(fkn), m_loop(loop)
    {
    }

    /// Return true if this is still registered in libev.
    bool active() const
    {
        return m_self.get() != nullptr;
    }

    void cancel()
    {
        auto evTimeout = static_cast<struct ev_timer*>(this);
        ev_timer_stop(m_loop, evTimeout);
        m_self.reset();
    }

  private:
    void setup(double delay, std::shared_ptr<TimeoutWatcher>& ptr)
    {
        struct ev_timer* evTimeout = this;
        ev_timer_init(evTimeout, timeout_cb, delay, 0.0);
        m_self = ptr;
        ev_timer_start(m_loop, evTimeout);
    }

    void cb(struct ev_loop* loop, int revents)
    {
        bool res = m_fkn();
        if (!res)
        {
            auto evTimeout = static_cast<struct ev_timer*>(this);
            ev_timer_stop(loop, evTimeout);
            m_self.reset();
        }
    }

    static void timeout_cb(struct ev_loop* loop, struct ev_timer* w,
                           int revents)
    {
        static_cast<TimeoutWatcher*>(w)->cb(loop, revents);
    }

    std::function<bool()> m_fkn;
    struct ev_loop* m_loop;

    // As long as we are registered with the event loop, keep
    // a reference around to make sure we are not deleted.
    std::shared_ptr<TimeoutWatcher> m_self;
};

#endif /* SRC_EVENTWRAPPER_TIMEOUTWATCHER_H_ */
