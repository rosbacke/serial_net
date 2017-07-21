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
 * PosixTunTapReal.cpp
 *
 *  Created on: 1 nov. 2016
 *      Author: mikaelr
 */

#include "PosixTunTapReal.h"

#include <arpa/inet.h>

#include <linux/if.h>
#include <linux/if_tun.h>

#include <sys/ioctl.h>

int
PosixTunTapReal::ioctl_TUNSETIFF(int fd, void* ifr_p)
{
    return ::ioctl(fd, TUNSETIFF, ifr_p);
}

/**
 * ioctl for accessing netdevice flags.
 */
int
PosixTunTapReal::ioctl_SIOCSIFFLAGS(int fd, void* ifr_p)
{
    return ::ioctl(fd, SIOCSIFFLAGS, ifr_p);
}

/**
 * ioctl for accessing netdevice flags.
 */
int
PosixTunTapReal::ioctl_SIOCGIFFLAGS(int fd, void* ifr_p)
{
    return ::ioctl(fd, SIOCGIFFLAGS, ifr_p);
}

int
PosixTunTapReal::ioctl_TUNSETOWNER(int fd, uid_t uid)
{
    return ::ioctl(fd, TUNSETOWNER, uid);
}

int
PosixTunTapReal::ioctl_TUNSETGROUP(int fd, gid_t gid)
{
    return ::ioctl(fd, TUNSETGROUP, gid);
}

int
PosixTunTapReal::ioctl_TUNSETPERSIST(int fd, int persist)
{
    return ::ioctl(fd, TUNSETPERSIST, persist);
}

int
PosixTunTapReal::socket(int domain, int type, int protocol)
{
    return ::socket(domain, type, protocol);
}
