/*
 * PosixFileIfReal_test.cpp
 *
 *  Created on: 30 okt. 2016
 *      Author: mikaelr
 */

#include "eventwrapper/EventLoop.h"
#include <gtest/gtest.h>

TEST(EventWrapper, MakeSureTestBuilds)
{
	EXPECT_EQ(1, 1);
}

TEST(EventWrapper, ConstructLoop)
{
	EventLoop loop;
}

TEST(EventWrapper, makeSureTimerWorks)
{
	EventLoop loop;

	double t1 = loop.now();
	EXPECT_GT(t1, 1e9);
	// Current time since 1970 is well above 1e9 today.

	usleep(10000);
	double t2 = loop.now();

	// Without update, should have the same time.
	EXPECT_LE(t2 - t1, 0.001);

	// After update, > 10ms should have passed.
	loop.updateNow();
	double t3 = loop.now();

	// std::cout << t3 - t1 << std::endl;
	EXPECT_GT(t3 - t1, 0.010);
}

TEST(EventWrapper, checkTimeoutCB)
{
	EventLoop loop;
	bool cbDone = false;

	double t1 = loop.now();
	double t2 = 0.0;

	loop.onTimeout(0.01, [&]()
	{	cbDone = true; t2 = loop.now(); return false; });

	EXPECT_EQ(cbDone, false);

	// Havn't started the loop. Should still be local time.
	double t3 = loop.now();
	EXPECT_EQ(t3 - t1, 0.0);

	loop.run();

	// Timer should have expired.
	EXPECT_EQ(cbDone, true);
	EXPECT_GT( t2 - t1, 0.01);
}

TEST(EventWrapper, checkSignalCB)
{
	EventLoop loop;
	bool cbSignalDone = false;

	double t1 = loop.now();
	double t2 = 0.0;


	loop.onSignal(SIGINT, [&]()
	{	cbSignalDone = true;
		t2 = loop.now();
		return false;
	});

	loop.onTimeout(0.01, [&]()
	{	int result = ::raise(SIGINT);
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


	loop.onReadable(pipeFd[0], [&]()
	{	cbReadDone = true;
		t2 = loop.now();
		return false;
	});

	loop.onTimeout(0.01, [&]()
	{	int res = ::write(pipeFd[1], "Test!", 5);
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


int main(int ac, char* av[])
{
  testing::InitGoogleTest(&ac, av);
  return RUN_ALL_TESTS();
}
