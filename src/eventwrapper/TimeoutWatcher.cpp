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
 * TimeoutWatcher.cpp
 *
 *  Created on: 3 jan. 2017
 *      Author: mikaelr
 */

#include "TimeoutWatcher.h"
#include "EventLoop.h"

TimeoutData::TimeoutData(EventLoop& ev) : m_loop(ev.m_loop)
{
}

void
TimeoutData::cancel()
{
    auto evTimeout = static_cast<struct ev_timer*>(this);
    ev_timer_stop(m_loop, evTimeout);
    m_active = false;
}

void
TimeoutData::cb(struct ev_loop* loop, int revents)
{
    if (m_active)
    {
        cancel();
        m_fkn();
        if (m_watcher)
        {
            m_watcher->clear();
        }
    }
}
