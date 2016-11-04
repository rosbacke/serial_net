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
 * PosixSerielIfReal.h
 *
 *  Created on: 29 okt. 2016
 *      Author: mikaelr
 */

#ifndef SRC_HAL_POSIXSERIALREAL_H_
#define SRC_HAL_POSIXSERIALREAL_H_

#include "PosixIf.h"

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

class PosixSerialReal final : public PosixSerialIf
{
  public:
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
        return ::ioctl(fd, TIOCMGET, status_p);
    }
    int ioctl_TIOCMSET(int fd, int* status_p) final
    {
        return ::ioctl(fd, TIOCMSET, status_p);
    }
    int ioctl_TIOCGRS485(int fd, void* status_p) const final
    {
        return ::ioctl(fd, TIOCGRS485, status_p);
    }
    int ioctl_TIOCSRS485(int fd, void* status_p) final
    {
        return ::ioctl(fd, TIOCSRS485, status_p);
    }
};

#endif /* SRC_HAL_POSIXSERIALREAL_H_ */
