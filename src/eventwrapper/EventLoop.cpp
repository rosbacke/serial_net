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
 * MainLoop.cpp
 *
 *  Created on: 1 jan. 2017
 *      Author: mikaelr
 */

#include "EventLoop.h"

#include <ev.h>
#include <memory>

EventLoop::EventLoop()
{
    setup();
}

EventLoop::~EventLoop()
{
}

void
EventLoop::setup()
{
    static struct ev_loop* loop = ::ev_default_loop(0);
    m_loop = loop;
}

std::shared_ptr<IoWatcher>
EventLoop::onReadable(int fd, std::function<bool()> cb)
{
    return IoWatcher::mkIoWatch(fd, cb, m_loop);
}

std::shared_ptr<TimeoutWatcher>
EventLoop::onTimeout(double delay, std::function<bool()> cb)
{
    return TimeoutWatcher::mkTimeoutWatch(delay, cb, m_loop);
}

std::shared_ptr<TimeoutWatcher>
EventLoop::onAvailable(std::function<bool()> cb)
{
    return TimeoutWatcher::mkTimeoutWatch(0.0, cb, m_loop);
}

std::shared_ptr<SignalWatcher>
EventLoop::onSignal(int signal, std::function<bool()> cb)
{
    return SignalWatcher::mkSignalWatch(signal, cb, m_loop);
}

void
EventLoop::run()
{
    ev_run(m_loop, 0);
}

void
EventLoop::stop()
{
    ev_break(m_loop, EVBREAK_ALL);
}

double
EventLoop::now()
{
    return ev_now(m_loop);
}

void
EventLoop::updateNow()
{
    ev_now_update(m_loop);
}
