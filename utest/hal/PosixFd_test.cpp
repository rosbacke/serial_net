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

