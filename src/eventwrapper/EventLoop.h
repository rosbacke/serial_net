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
 * EventLoop.h
 *
 *  Created on: 1 jan. 2017
 *      Author: mikaelr
 */

#ifndef SRC_EVENTWRAPPER_EVENTLOOP_H_
#define SRC_EVENTWRAPPER_EVENTLOOP_H_

#include "eventwrapper/IoWatcher.h"
#include "eventwrapper/SignalWatcher.h"
#include "eventwrapper/TimeoutWatcher.h"
#include <functional>

extern "C" {
struct ev_loop;
}

/**
 * Wrap libev into C++ constructs. Set up Watchers for events. When the event
 * happens, call the given callback.
 *
 * Callbacks: Do user defined code. Return false, stop watching the event,
 * return true, continue to watch the given event.
 *
 * The watcher is returned via a shared_ptr. As long as the watcher is active
 * the object is kept alive internally.
 */
class EventLoop
{
  public:
    EventLoop();
    ~EventLoop();

    /**
     * Wait for a given file descriptor being readable.
     */
    std::shared_ptr<IoWatcher> onReadable(int fd, std::function<bool()> cb);

    /**
     * Wait for a given time before calling the callback.
     */
    std::shared_ptr<TimeoutWatcher> onTimeout(double delay,
                                              std::function<bool()> cb);

    /**
     * Wait for a given Unix process signal.
     */
    std::shared_ptr<SignalWatcher> onSignal(int signal,
                                            std::function<bool()> cb);

    // Enter the event loop to start watching for events.
    void run();

    // Request that the event loop is stopped.
    void stop();

    // Return the time when the loop event processing started.
    // (seconds since epoch.
    double now();

    // Update the 'now' value. Normally no needed, but can be useful during long
    // running callbacks.
    void updateNow();

  private:
    void setup();

    struct ev_loop* m_loop;
};

#endif /* SRC_EVENTWRAPPER_EVENTLOOP_H_ */
