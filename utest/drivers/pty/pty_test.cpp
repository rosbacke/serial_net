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

#include "drivers/pty/PtyRawHostDriver.h"

#include "hal/PosixFileReal.h"

#include <gtest/gtest.h>

#include <chrono>
#include <fstream>

#include <cstring>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <linux/serial.h>

#include "hal/PosixSleepReal.h"

#if 0
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

    // Note: even though m_status is a bitmask, the ioctl define it as a signed
    // int.
    int m_status = 0;
    mutable int m_cnt = 0;
    int m_return = 0;
    mutable int m_fd = -1;
};
#endif

TEST(TestPty, Construction)
{
    PosixFileReal pfr;
    PtyRawHostDriver rhd(&pfr, nullptr);
}

TEST(TestPty, test_posix_pty_setup_functions)
{
    int fd = PtyRawHostDriver::openPty();
    EXPECT_GE(fd, 0);

    auto name = PtyRawHostDriver::ptyName(fd);
    std::cout << "Name: " << name;

    // Make sure the name exist in the file system.
    int fd2 = ::open(name.c_str(), O_RDONLY);
    EXPECT_GE(fd2, 0);
    ::close(fd2);

    // Close Pty.
    ::close(fd);

    // And the file system name should be gone from file system.
    fd2 = ::open(name.c_str(), O_RDONLY);
    EXPECT_EQ(fd2, -1);
}

int
main(int ac, char* av[])
{
    testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}
