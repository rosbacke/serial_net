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
 * PosixSerialUtils_test.cpp
 *
 *  Created on: 2 nov. 2016
 *      Author: mikaelr
 */


#include "drivers/serial/PosixSerialUtils.h"

#include <gtest/gtest.h>

#include <fstream>
#include <chrono>


#include <cstring>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <linux/serial.h>

#include "hal/PosixSleepReal.h"



class PosixSerialMock final : public PosixSerialIf
{
  public:
	PosixSerialMock()
  	{
		::memset(&m_rs485, 0, sizeof(m_rs485));
  	}
    int cfsetospeed(struct termios* termios_p, speed_t speed) final
    {
        return ::cfsetospeed(termios_p, speed);
    }
    int cfsetispeed(struct termios* termios_p, speed_t speed) final
    {
        return ::cfsetispeed(termios_p, speed);
    }
    int tcsetattr(int fd, int optional_actions,
                  const struct termios* termios_p) final
    {
        return ::tcsetattr(fd, optional_actions, termios_p);
    }

    int ioctl_TIOCMGET(int fd, int* status_p) const final
    {
    	m_fd = fd;
    	*status_p = m_status;
    	m_cnt++;
        return m_return;
    }

    int ioctl_TIOCMSET(int fd, int* status_p) final
    {
       	m_fd = fd;
       	m_status = *status_p;
       	m_cnt++;
       	return m_return;
    }
    int ioctl_TIOCGRS485(int fd, void* status_p) const final
   	{
       	m_fd = fd;
    	::memcpy(status_p, &m_rs485, sizeof m_rs485);
    	m_cnt++;
    	return m_return;
  	}

    int ioctl_TIOCSRS485(int fd, void* status_p) final
    {
       	m_fd = fd;
    	::memcpy(&m_rs485, status_p, sizeof m_rs485);
    	m_cnt++;
    	return m_return;
  	}

	struct serial_rs485 m_rs485;


    int m_speed = 0;

    // Note: even though m_status is a bitmask, the ioctl define it as a signed int.
    int m_status = 0;
    mutable int m_cnt = 0;
    int m_return = 0;
    mutable int m_fd = -1;
};


TEST(SetRTS, SetRTS)
{
	{
		PosixSerialMock m;
		int fd = 1;
		m.m_return = 5;
		m.m_status = 0x7fff;
		int res = PosixSerialUtils::setRTS(m, fd, PosixSerialUtils::IOState::negated);
		EXPECT_EQ(m.m_fd, 1);
		EXPECT_EQ(m.m_status, 0x7fff & ~TIOCM_RTS);
		EXPECT_EQ(res, 5);
	}
	{
		PosixSerialMock m;
		int fd = 3;
		m.m_return = 7;
		m.m_status = 0x11;
		int res = PosixSerialUtils::setRTS(m, fd, PosixSerialUtils::IOState::asserted);
		EXPECT_EQ(m.m_fd, 3);
		EXPECT_EQ(m.m_status, 0x11 | TIOCM_RTS);
		EXPECT_EQ(res, 7);
	}
}

TEST(SetRS485, SetRS485_1)
{
	{
		PosixSerialMock m;
		int fd = 1;
		m.m_return = 5;
		m.m_status = 0x7fff;
		int res = PosixSerialUtils::setRS485Mode(m, fd, false, false);

		EXPECT_EQ(m.m_fd, 1);
		EXPECT_EQ(m.m_rs485.flags & SER_RS485_ENABLED, 0u);
		EXPECT_EQ(res, 5);
	}
	{
		PosixSerialMock m;
		int fd = 2;
		m.m_return = 3;
		m.m_status = 0x7fff;
		int res = PosixSerialUtils::setRS485Mode(m, fd, true, false);

		EXPECT_EQ(m.m_fd, 2);
		uint32_t mask = SER_RS485_ENABLED | SER_RS485_RTS_ON_SEND;
		EXPECT_EQ(m.m_rs485.flags & mask, mask);
		EXPECT_EQ(res, 3);
	}
}

int main(int ac, char* av[])
{
  testing::InitGoogleTest(&ac, av);
  return RUN_ALL_TESTS();
}


