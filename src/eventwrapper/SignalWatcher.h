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
 * SignalWatcher.h
 *
 *  Created on: 3 jan. 2017
 *      Author: mikaelr
 */

#ifndef SRC_EVENTWRAPPER_SIGNALWATCHER_H_
#define SRC_EVENTWRAPPER_SIGNALWATCHER_H_

#include <ev.h>
#include <memory>

class SignalWatcher : ev_signal
{
  public:
    static std::shared_ptr<SignalWatcher>
    mkSignalWatch(struct ev_loop* loop, int signal, std::function<bool()> fkn)
    {
        auto t = std::make_shared<SignalWatcher>(fkn);
        t->setup(signal, loop, t);
        return t;
    }

    ~SignalWatcher(){};

    SignalWatcher(std::function<bool()> fkn) : m_fkn(fkn)
    {
    }

    /// Return true if this is still registered in libev.
    bool active() const
    {
        return m_self.get() != nullptr;
    }

  private:
    void setup(int signal, struct ev_loop* loop,
               std::shared_ptr<SignalWatcher>& ptr)
    {
        struct ev_signal* evSignal = this;
        ev_signal_init(evSignal, signal_cb, signal);
        m_self = ptr;
        ev_signal_start(loop, evSignal);
    }

    void cb(struct ev_loop* loop, int revents)
    {
        bool res = m_fkn();
        if (!res)
        {
            auto evSignal = static_cast<struct ev_signal*>(this);
            ev_signal_stop(loop, evSignal);
            m_self.reset();
        }
    }

    static void signal_cb(struct ev_loop* loop, struct ev_signal* w,
                          int revents)
    {
        static_cast<SignalWatcher*>(w)->cb(loop, revents);
    }

    std::function<bool()> m_fkn;

    // As long as we are registered with the event loop, keep
    // a reference around to make sure we are not deleted.
    std::shared_ptr<SignalWatcher> m_self;
};

#endif /* SRC_EVENTWRAPPER_SIGNALWATCHER_H_ */
