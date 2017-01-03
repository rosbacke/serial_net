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

#include "hal/PosixIf.h"
#include "utility/Log.h"

TapHostDriver::TapHostDriver(AddressCache* ac, PosixFileIf* pfi,
                             PosixTunTapIf* ptti, const std::string& on_if_up,
                             const std::string& on_if_down)
    : m_tap(ac, pfi), m_tun_fd(pfi), m_pfi(pfi), m_ptti(ptti),
      m_on_if_up(on_if_up), m_on_if_down(on_if_down), m_ifUp(false)
{
}

TapHostDriver::~TapHostDriver()
{
    setTapIfUpDown(m_ptti, false);
}

int
TapHostDriver::tun_alloc(std::string& dev, unsigned tunFlags)
{
    struct ifreq ifr;
    int fd, err;

    LOG_DEBUG << "tun_alloc";

    if ((fd = m_pfi->open("/dev/net/tun", O_RDWR)) < 0)
    {
        LOG_ERROR << "Can not open /dev/net/tun error.";
        return -1;
    }
    ::memset(&ifr, 0, sizeof(ifr));

    /* Flags: IFF_TUN   - TUN device (no Ethernet headers)
     *        IFF_TAP   - TAP device
     *
     *        IFF_NO_PI - Do not provide packet information
     */
    ifr.ifr_flags = tunFlags;
    ::strncpy(ifr.ifr_name, dev.c_str(), IFNAMSIZ);

    if ((err = m_ptti->ioctl_TUNSETIFF(fd, (void*)&ifr)) < 0)
    {
        LOG_ERROR << "Failed to ioctl on tun device.." << errno;
        m_pfi->close(fd);
        return err;
    }
    dev = std::string(ifr.ifr_name);
    LOG_DEBUG << "tun_alloc success : fd " << fd;

    return fd;
}

void
TapHostDriver::startTransfer(MsgHostIf* txIf, EventLoop& loop)
{
    LOG_DEBUG << "startTransfer done";
    m_tap.setTx(txIf);
    setupCallback(loop);
    txIf->setRxHandler(this);
    txIf->setAddrUpdateHandler(this);
    msgHostRx_newAddr(txIf->msgHostTx_clientAddress());
}

void
TapHostDriver::setupCallback(EventLoop& mainLoop)
{
    std::string tun_name("tap0");

    m_tun_fd.set(tun_alloc(tun_name, IFF_TAP)); /* tap interface */

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

void
TapHostDriver::msgHostRx_newAddr(LocalAddress addr)
{

    const bool newIfUp = (addr != LocalAddress::null_addr);
    if (newIfUp != m_ifUp)
    {
        setTapIfUpDown(m_ptti, newIfUp);
        if (newIfUp && m_on_if_up != "")
        {
            m_pfi->system(m_on_if_up.c_str());
        }
        else if (!newIfUp && m_on_if_down != "")
        {
            m_pfi->system(m_on_if_down.c_str());
        }
        m_ifUp = newIfUp;
    }
}

void
TapHostDriver::setTapIfUpDown(PosixTunTapIf* posix, bool up)
{
    int fd = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct ifreq ethreq;

    // See: 'man 7 netdevice' for details.
    struct ifreq req;
    int err;

    memset(&req, 0, sizeof(ethreq));

    /* set the name of the interface we wish to check */
    strncpy(req.ifr_name, "tap0", IFNAMSIZ);

    LOG_INFO << "Try ioctl_SIOCGIFFLAGS : fd:" << fd;

    err = posix->ioctl_SIOCGIFFLAGS(fd, (void*)&req);
    if (err == -1)
    {
        LOG_ERROR << "Error with ioctl_SIOCGIFFLAGS : " << ::strerror(errno);
        throw std::runtime_error("Failed getting tap interface flags.");
    }
    auto oldVal = req.ifr_ifru.ifru_flags;
    if (up)
    {
        req.ifr_ifru.ifru_flags |= IFF_UP;
    }
    else
    {
        req.ifr_ifru.ifru_flags &= ~IFF_UP;
    }
    if (req.ifr_ifru.ifru_flags != oldVal)
    {
        err = posix->ioctl_SIOCSIFFLAGS(fd, (void*)&req);
        if (err == -1)
        {
            throw std::runtime_error("Failed setting tap interface flags.");
        }
    }
}
