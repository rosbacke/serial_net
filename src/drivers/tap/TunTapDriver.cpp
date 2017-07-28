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
 * TunTapDriver.cpp
 *
 *  Created on: 4 juli 2017
 *      Author: mikaelr
 */

#include "TunTapDriver.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "hal/PosixFd.h"
#include "hal/PosixIf.h"
#include "utility/Log.h"

#include <ctype.h>
#include <grp.h>
#include <pwd.h>

TunTapDriver::TunTapDriver(PosixFileIf* pfi, PosixTunTapIf* ptti)
    : m_pfi(pfi), m_ptti(ptti)
{
}

TunTapDriver::~TunTapDriver()
{
    // TODO Auto-generated destructor stub
}

std::string
TunTapDriver::toString(IfType t)
{
    switch (t)
    {
    case IfType::tap:
        return "tap";
    case IfType::tun:
        return "tun";
    }
    return "unknown";
}

void
TunTapDriver::setUserGroup(int fd, std::string user, std::string group)
{
    assert(fd >= 0);

    struct passwd* pwEntry = ::getpwnam(user.c_str());
    if (pwEntry)
    {
        m_ptti->ioctl_TUNSETOWNER(fd, pwEntry->pw_uid);
    }

    struct group* grpEntry = ::getgrnam(group.c_str());
    if (grpEntry)
    {
        m_ptti->ioctl_TUNSETGROUP(fd, grpEntry->gr_gid);
    }
}

void
TunTapDriver::persist(int fd, bool persist)
{
    m_ptti->ioctl_TUNSETPERSIST(fd, persist ? 1 : 0);
}

int
TunTapDriver::tuntap_alloc(std::string& dev, IfType type)
{
    struct ifreq ifr;
    int fd, err;

    LOG_DEBUG << "tuntap_alloc:" << toString(type);

    if ((fd = m_pfi->open("/dev/net/tun", O_RDWR)) < 0)
    {
        LOG_ERROR << "Can not open /dev/net/tun error:" << ::strerror(errno);
        return -1;
    }
    ::memset(&ifr, 0, sizeof(ifr));

    /* Flags: IFF_TUN   - TUN device (no Ethernet headers)
     *        IFF_TAP   - TAP device
     *
     *        IFF_NO_PI - Do not provide packet information
     */
    unsigned flags = type == IfType::tap ? IFF_TAP : IFF_TUN;
    ifr.ifr_flags = flags;
    ::strncpy(ifr.ifr_name, dev.c_str(), IFNAMSIZ);

    if ((err = m_ptti->ioctl_TUNSETIFF(fd, (void*)&ifr)) < 0)
    {
        LOG_ERROR << "Failed to ioctl on tun device : " << ::strerror(errno);
        m_pfi->close(fd);
        return err;
    }
    dev = std::string(ifr.ifr_name);
    LOG_DEBUG << "tun_alloc success : fd " << fd;

    return fd;
}

void
TunTapDriver::setIfUpDown(bool up, std::string tapName)
{
    if (tapName.size() > IFNAMSIZ || tapName.size() == 0)
    {
        LOG_ERROR << "Max length for TAP device name is :" << (int)IFNAMSIZ;
        throw std::runtime_error("Tap device name length error.");
    }
    auto fd = PosixFd::makeFd(m_ptti->socket(PF_INET, SOCK_STREAM, IPPROTO_TCP),
                              m_pfi);
    if (fd == -1)
    {
        LOG_ERROR << "Failed opening socket for TAP handling: "
                  << strerror(errno);
        throw std::runtime_error("Failed opening socket for TAP handling.");
    }

    struct ifreq ethreq;

    // See: 'man 7 netdevice' for details.
    struct ifreq req;
    int err;

    memset(&req, 0, sizeof(ethreq));

    /* set the name of the interface we wish to check */
    strncpy(req.ifr_name, tapName.c_str(), IFNAMSIZ);

    err = m_ptti->ioctl_SIOCGIFFLAGS(fd, (void*)&req);
    if (err == -1)
    {
        LOG_ERROR << "Error with ioctl_SIOCGIFFLAGS : " << ::strerror(errno);
        throw std::runtime_error("Failed getting tap interface flags.");
    }
    auto* fp = &req.ifr_ifru.ifru_flags;
    auto oldVal = *fp;
    if (up)
    {
        *fp |= IFF_UP;
    }
    else
    {
        *fp &= ~IFF_UP;
    }
    if (*fp != oldVal)
    {
        err = m_ptti->ioctl_SIOCSIFFLAGS(fd, (void*)&req);
        if (err == -1)
        {
            throw std::runtime_error("Failed setting tap interface flags.");
        }
    }
}
