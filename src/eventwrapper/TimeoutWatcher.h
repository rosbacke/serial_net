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

class TimeoutWatcher;
class EventLoop;

/**
 * Class wrapping the event time watcher. Must not be moved.
 * Will clean up time watcher upon destruction.
 */
class TimeoutData : ev_timer
{
  public:
    TimeoutData(struct ev_loop* loop) : m_loop(loop)
    {
    }
    TimeoutData(EventLoop& ev);
    TimeoutData(TimeoutData&& td) = delete;
    TimeoutData(const TimeoutData& td) = delete;
    TimeoutData& operator=(const TimeoutData& td) = delete;
    TimeoutData& operator=(TimeoutData&& td) = delete;

    void start(double timespan, std::function<void()> fkn)
    {
        m_fkn = fkn;
        setup(timespan);
    }

    ~TimeoutData()
    {
        if (m_active)
        {
            cancel();
        }
    }

    void cancel();

    void setWatcher(TimeoutWatcher* watcher)
    {
        m_watcher = watcher;
    }

    bool active() const
    {
        return m_active;
    }

  private:
    void setup(double timespan)
    {
        m_active = true;
        struct ev_timer* evTimeout = this;
        ev_timer_init(evTimeout, timeout_cb, timespan, 0.0);
        ev_timer_start(m_loop, evTimeout);
    }

    void cb(struct ev_loop* loop, int revents);

    static void timeout_cb(struct ev_loop* loop, struct ev_timer* w,
                           int revents)
    {
        static_cast<TimeoutData*>(w)->cb(loop, revents);
    }

    TimeoutWatcher* m_watcher = nullptr;
    std::function<void()> m_fkn;
    struct ev_loop* m_loop = nullptr;
    bool m_active = false;
};

/**
 * Wrapper for TimeoutData. Keeps a reference to self that ensures it
 * will stay alive even if the client drops the object before timeout.
 */
class TimeoutWatcher
{
  public:
    static std::shared_ptr<TimeoutWatcher>
    mkTimeoutWatch(struct ev_loop* loop, double delay,
                   std::function<bool()> fkn)
    {
        auto t = std::make_shared<TimeoutWatcher>(loop);
        t->setup(fkn, delay, t);
        return t;
    }

    ~TimeoutWatcher(){};

    TimeoutWatcher(struct ev_loop* loop) : m_data(loop)
    {
    }

    /// Return true if this is still registered in libev.
    bool active() const
    {
        return m_data.active();
    }

    void cancel()
    {
        m_data.cancel();
        m_self.reset();
    }

    void clear()
    {
        m_self.reset();
    }

  private:
    void setup(std::function<bool()> fkn, double delay,
               std::shared_ptr<TimeoutWatcher>& ptr)
    {
        m_self = ptr;
        m_data.start(delay, fkn);
        m_data.setWatcher(this);
    }

    TimeoutData m_data;

    // As long as we are registered with the event loop, keep
    // a reference around to make sure we are not deleted.
    std::shared_ptr<TimeoutWatcher> m_self;
};

#endif /* SRC_EVENTWRAPPER_TIMEOUTWATCHER_H_ */
