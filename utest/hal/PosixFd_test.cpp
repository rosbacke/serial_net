/*
 * PosixFileIfReal_test.cpp
 *
 *  Created on: 30 okt. 2016
 *      Author: mikaelr
 */

#include "hal/PosixFileReal.h"

#include <gtest/gtest.h>

#include <fstream>
#include <chrono>
#include "hal/PosixSleepReal.h"
#include "hal/PosixFileFake.h"

#include "hal/PosixFd.h"


// Test open, read, close.
TEST(PosixFd, PosixFd)
{
	PosixFileFake pff;
	pff.m_fd = 0;
	{
		pff.m_cnt = 0;
		PosixFd fd = PosixFd::makeFd(3, &pff);
		EXPECT_EQ(pff.m_cnt, 0);
		EXPECT_EQ(fd.get(), 3);

		int t = 0;
		t = fd;
		EXPECT_EQ(t, 3);

		EXPECT_EQ(pff.m_fd, 0);
	}
	EXPECT_EQ(pff.m_fd, 3); // Should have called close with fd == 3.
	EXPECT_EQ(pff.m_cnt, 1);
}

