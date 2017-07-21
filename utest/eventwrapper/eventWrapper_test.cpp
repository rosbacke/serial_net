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

#include "eventwrapper/EventLoop.h"
#include <gtest/gtest.h>

TEST(EventWrapper, MakeSureTestsBuilds)
{
    EXPECT_EQ(1, 1);
}

TEST(EventWrapper, EventLoop_can_be_constructed)
{
    EventLoop loop;
}

TEST(EventWrapper, test_that_nowfkn_gives_reasonable_values)
{
    EventLoop loop;

    double t1 = loop.now();

    // Current time since 1970 is well above 1e9 today.
    EXPECT_GT(t1, 1e9);
}

TEST(EventWrapper, test_that_nowfkn_increase_with_time_after_update)
{
    EventLoop loop;

    double t1 = loop.now();
    usleep(2000);
    double t2 = loop.now();

    // Without update, should have the same time. (bar rounding errors.)
    EXPECT_LE(t2 - t1, 0.001);

    // After update, > 10ms should have passed.
    loop.updateNow();
    double t3 = loop.now();

    EXPECT_GT(t3 - t1, 0.002);
}

TEST(EventWrapper, test_that_enqueued_fkn_run_after_runfkn_is_called)
{
    EventLoop loop;
    double t1 = loop.now();
    bool cbDone = false;
    loop.onAvailable([&]() {
        cbDone = true;
        return false;
    });
    // Make sure we havn't run the lambda function yet.
    EXPECT_FALSE(cbDone);

    loop.run();
    double t2 = loop.now();

    // And after entering the event loop, the lamda shoud have run.
    EXPECT_TRUE(cbDone);

    // And within negligible time.
    EXPECT_LE(t2 - t1, 0.005);
}

TEST(EventWrapper, checkTimeoutCB)
{
    EventLoop loop;
    bool cbDone = false;

    double t1 = loop.now();
    double t2 = 0.0;

    loop.onTimeout(0.01, [&]() {
        cbDone = true;
        t2 = loop.now();
        return false;
    });

    EXPECT_EQ(cbDone, false);

    // Havn't started the loop. Should still be local time.
    double t3 = loop.now();
    EXPECT_EQ(t3 - t1, 0.0);

    loop.run();

    // Timer should have expired.
    EXPECT_EQ(cbDone, true);

    // And enough time shuld have passed.
    EXPECT_GT(t2 - t1, 0.01);
}

TEST(EventWrapper, checkSignalCB)
{
    EventLoop loop;
    bool cbSignalDone = false;

    double t1 = loop.now();
    double t2 = 0.0;

    loop.onSignal(SIGINT, [&]() {
        cbSignalDone = true;
        t2 = loop.now();
        return false;
    });

    loop.onTimeout(0.01, [&]() {
        int result = ::raise(SIGINT);
        EXPECT_EQ(result, 0);
        return false;
    });

    EXPECT_EQ(cbSignalDone, false);

    loop.run();

    double t3 = loop.now();
    EXPECT_GT(t3 - t1, 0.01);
    EXPECT_GT(t2 - t1, 0.01);

    // Note: the signal should arrive relatively close in time, not by
    // pressing Ctrl-C
    EXPECT_LE(t2 - t1, 0.1);

    // Timer should have expired.
    EXPECT_EQ(cbSignalDone, true);
}

TEST(EventWrapper, checkReadFileFd)
{
    EventLoop loop;
    bool cbReadDone = false;

    double t1 = loop.now();
    double t2 = 0.0;

    int pipeFd[2];
    int res = ::pipe(pipeFd);
    EXPECT_EQ(res, 0);

    // pipeFd[0] : read descriptor.
    // pipeFd[1] : write descriptor.

    loop.onReadable(pipeFd[0], [&]() {
        cbReadDone = true;
        t2 = loop.now();
        return false;
    });

    loop.onTimeout(0.01, [&]() {
        int res = ::write(pipeFd[1], "Test!", 5);
        EXPECT_EQ(res, 5);
        return false;
    });

    EXPECT_EQ(cbReadDone, false);

    loop.run();

    double t3 = loop.now();
    EXPECT_GT(t3 - t1, 0.01);
    EXPECT_GT(t2 - t1, 0.01);

    // Timer should have expired.
    EXPECT_EQ(cbReadDone, true);

    res = ::close(pipeFd[0]);
    EXPECT_EQ(res, 0);

    res = ::close(pipeFd[1]);
    EXPECT_EQ(res, 0);
}

int
main(int ac, char* av[])
{
    testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}
