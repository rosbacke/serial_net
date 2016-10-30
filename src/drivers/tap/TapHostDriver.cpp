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
 * TapHostDriver.cpp
 *
 *  Created on: 23 okt. 2016
 *      Author: mikaelr
 */

#include "TapHostDriver.h"

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

#include "utility/Log.h"

TapHostDriver::TapHostDriver(int myAddr, AddressCache* ac)
    : m_tap(myAddr, ac), m_tun_fd(-1)
{
}

TapHostDriver::~TapHostDriver()
{
    if (m_tun_fd > 0)
    {
        ::close(m_tun_fd);
    }
}

namespace
{

int
tun_alloc(char* dev, unsigned tunFlags)
{
    struct ifreq ifr;
    int fd, err;

    LOG_DEBUG << "tun_alloc";

    if ((fd = open("/dev/net/tun", O_RDWR)) < 0)
    {
        LOG_ERROR << "Can not open /dev/net/tun error.";
        return -1;
    }
    memset(&ifr, 0, sizeof(ifr));

    /* Flags: IFF_TUN   - TUN device (no Ethernet headers)
     *        IFF_TAP   - TAP device
     *
     *        IFF_NO_PI - Do not provide packet information
     */
    ifr.ifr_flags = tunFlags;
    if (*dev)
        strncpy(ifr.ifr_name, dev, IFNAMSIZ);

    if ((err = ioctl(fd, TUNSETIFF, (void*)&ifr)) < 0)
    {
        LOG_ERROR << "Failed to ioctl on tun device.." << errno;
        close(fd);
        return err;
    }
    strcpy(dev, ifr.ifr_name);
    LOG_DEBUG << "tun_alloc success : fd " << fd;

    return fd;
}
}

void
TapHostDriver::startTransfer(MsgHostIf::TxIf* txIf, React::Loop& loop)
{
    LOG_DEBUG << "startTransfer done";
    m_tap.setTx(txIf);
    setupCallback(loop);
}

void
TapHostDriver::setupCallback(React::Loop& mainLoop)
{
    char tun_name[IFNAMSIZ];

    /* Connect to the device */
    strcpy(tun_name, "tap0");
    m_tun_fd = tun_alloc(tun_name, IFF_TAP); /* tap interface */

    if (m_tun_fd < 0)
    {
        throw std::runtime_error("Failed starting tap interface.");
    }

    // we'd like to be notified when input is available on stdin
    mainLoop.onReadable(m_tun_fd, [this]() -> bool {
        LOG_DEBUG << "Read from stdin.";
        m_tap.doRead(m_tun_fd);

        // return true, so that we also return future read events
        return true;
    });
}
