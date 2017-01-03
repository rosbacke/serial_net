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
 * IoWatcher.h
 *
 *  Created on: 2 jan. 2017
 *      Author: mikaelr
 */

#ifndef SRC_EVENTWRAPPER_IOWATCHER_H_
#define SRC_EVENTWRAPPER_IOWATCHER_H_

#include <ev.h>
#include <memory>

class IoWatcher : ev_io
{
  public:
    static std::shared_ptr<IoWatcher>
    mkIoWatch(int fd, std::function<bool()> fkn, struct ev_loop* loop)
    {
        auto t = std::make_shared<IoWatcher>(fkn);
        t->setup(fd, loop, t);
        return t;
    }

    ~IoWatcher(){};

    IoWatcher(std::function<bool()> fkn) : m_fkn(fkn)
    {
    }

    /// Return true if this is still registered in libev.
    bool active() const
    {
        return m_self.get() != nullptr;
    }

  private:
    void setup(int fd, struct ev_loop* loop, std::shared_ptr<IoWatcher>& ptr)
    {
        struct ev_io* evIo = this;
        ev_io_init(evIo, stdin_cb, fd, EV_READ);
        m_self = ptr;
        ev_io_start(loop, evIo);
    }

    void cb(struct ev_loop* loop, int revents)
    {
        bool res = m_fkn();
        if (!res)
        {
            auto evIo = static_cast<struct ev_io*>(this);
            ev_io_stop(loop, evIo);
            m_self.reset();
        }
    }

    static void stdin_cb(struct ev_loop* loop, struct ev_io* w, int revents)
    {
        static_cast<IoWatcher*>(w)->cb(loop, revents);
    }

    std::function<bool()> m_fkn;

    // As long as we are registered with the event loop, keep
    // a reference around to make sure we are not deleted.
    std::shared_ptr<IoWatcher> m_self;
};

#endif /* SRC_EVENTWRAPPER_IOWATCHER_H_ */
