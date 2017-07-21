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
 * PosixFileIfReal_test.cpp
 *
 *  Created on: 30 okt. 2016
 *      Author: mikaelr
 */

#include <gtest/gtest.h>

#include <chrono>
#include <fstream>

#include "eventwrapper/EventLoop.h"
#include "hal/PosixSleepReal.h"
#include "hal/TimeServiceEv.h"

TEST(TimeServiceEv, construction)
{
    EventLoop loop;
    TimeServiceEv ev(loop);
}

TEST(TimeServiceEv, now_time_counter_is_moving)
{
    EventLoop loop;
    TimeServiceEv ev(loop);

    PosixSleepReal ps;

    auto now1 = ev.now();
    ps.usleep(1000);
    loop.updateNow();
    auto now2 = ev.now();
    ASSERT_GT(now2, now1 + 0.001);
}

TEST(TimeServiceEv, check_makeTimeout_that_really_times_out)
{
    EventLoop loop;
    TimeServiceEv ev(loop);
    bool cbDone = false;

    double t1 = ev.now();
    double t2 = 0.0;

    auto timer = ev.makeTimeout(0.002, [&]() {
        cbDone = true;
        t2 = loop.now();
        return false;
    });

    EXPECT_EQ(cbDone, false);

    // Havn't started the loop. Should still be local time.
    double t3 = ev.now();
    EXPECT_EQ(t3 - t1, 0.0);

    loop.run();

    // Timer should have expired.
    EXPECT_EQ(cbDone, true);

    // And enough time should have passed.
    EXPECT_GT(t2 - t1, 0.002);
}

TEST(TimeServiceEv, check_makeTimeoutAbs_that_really_times_out)
{
    EventLoop loop;
    TimeServiceEv ev(loop);
    bool cbDone = false;

    double t1 = ev.now();
    double t2 = 0.0;

    auto timer = ev.makeTimeoutAbs(t1 + 0.002, [&]() {
        cbDone = true;
        t2 = ev.now();
        return false;
    });

    EXPECT_EQ(cbDone, false);

    // Havn't started the loop. Should still be local time.
    double t3 = ev.now();
    EXPECT_EQ(t3 - t1, 0.0);

    loop.run();

    // Timer should have expired.
    EXPECT_EQ(cbDone, true);

    // And enough time should have passed.
    EXPECT_GT(t2 - t1, 0.002);
}

TEST(TimeServiceEv, calling_cancel_will_not_call_callback)
{
    EventLoop loop;
    TimeServiceEv ev(loop);
    bool cbDone = false;

    double t1 = ev.now();
    double t2 = 0.0;

    auto timer = ev.makeTimeoutAbs(t1 + 0.002, [&]() {
        cbDone = true;
        t2 = ev.now();
    });

    EXPECT_EQ(cbDone, false);

    // Havn't started the loop. Should still be local time.
    double t3 = ev.now();
    EXPECT_EQ(t3 - t1, 0.0);

    timer.cancel();

    // No events registered. Should return immediately.
    loop.run();

    // No timer cb called.
    EXPECT_FALSE(cbDone);

    // And we should return immediately.
    EXPECT_LT(ev.now() - t1, 0.002);
}

TEST(TimeServiceEv, dropping_timer_object_will_cancel_timeout)
{
    EventLoop loop;
    TimeServiceEv ev(loop);
    bool cbDone = false;

    double t1 = ev.now();
    double t2 = 0.0;

    // auto timer =
    ev.makeTimeoutAbs(t1 + 0.002, [&]() {
        cbDone = true;
        t2 = ev.now();
    });

    EXPECT_EQ(cbDone, false);

    // Havn't started the loop. Should still be local time.
    double t3 = ev.now();
    EXPECT_EQ(t3 - t1, 0.0);

    // No events registered. Dropped when dropping 'timer' on the floor.
    loop.run();

    // No timer cb called.
    EXPECT_FALSE(cbDone);

    // And we should return immediately.
    EXPECT_LT(ev.now() - t1, 0.002);
}
