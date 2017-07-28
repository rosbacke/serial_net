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
 * TunHostDriver.cpp
 *
 *  Created on: 23 juli 2017
 *      Author: mikaelr
 */

#include "TunHostDriver.h"
#include "hal/PosixIf.h"

#include "utility/Log.h"

TunHostDriver::TunHostDriver(PosixFileIf* pfi,
                             std::unique_ptr<TunTapDriver>&& ttd)
    : m_tun_fd(pfi), m_pfi(pfi), m_tun(pfi), m_ifUp(false),
      m_tunTapDriver(std::move(ttd))
{
}

TunHostDriver::~TunHostDriver()
{
    if (m_ifUp)
    {
        m_tunTapDriver->setIfUpDown(false, m_tunName);
        m_ifUp = false;
    }
}

void
TunHostDriver::startTransfer(MsgHostIf* txIf, EventLoop& loop)
{
    LOG_DEBUG << "tun: startTransfer done";
    m_tun.setTx(txIf);
    setupCallback(loop);
    txIf->setRxHandler(this, ChannelType::tun_format);
    txIf->setAddrUpdateHandler(this);
    msgHostRx_newAddr(txIf->msgHostTx_clientAddress());
}

void
TunHostDriver::setupCallback(EventLoop& mainLoop)
{
    auto fd =
        m_tunTapDriver->tuntap_alloc(m_tunName, TunTapDriver::IfType::tun);
    m_tun_fd.set(fd); /* tun interface */

    if (m_tun_fd < 0)
    {
        throw std::runtime_error("Failed starting tun interface.");
    }
    LOG_DEBUG << "Setup read callback on TUN device, FD:" << m_tun_fd;

    // we'd like to be notified when input is available on stdin
    mainLoop.onReadable(m_tun_fd, [this]() -> bool {
        LOG_DEBUG << "TUN device ready to read.";
        m_tun.doRead2(m_tun_fd);

        // return true, so that we also return future read events
        return true;
    });
}

void
TunHostDriver::msgHostRx_newAddr(LocalAddress addr)
{

    const bool newIfUp = (addr != LocalAddress::null_addr);
    if (newIfUp != m_ifUp)
    {
        m_tunTapDriver->setIfUpDown(newIfUp, m_tunName);
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
