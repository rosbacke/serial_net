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

#include "hal/PosixIf.h"
#include "utility/Log.h"

TapHostDriver::TapHostDriver(AddressCache* ac, PosixFileIf* pfi,
                             std::unique_ptr<TunTapDriver>&& ttd)
    : m_tap(ac, pfi), m_tap_fd(pfi), m_pfi(pfi), m_ifUp(false),
      m_tunTapDriver(std::move(ttd))
{
}

TapHostDriver::~TapHostDriver()
{
    if (m_ifUp)
    {
        m_tunTapDriver->setIfUpDown(false, m_tapName);
        m_ifUp = false;
    }
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
    auto fd = m_tunTapDriver->tuntap_alloc(m_tapName);
    m_tap_fd.set(fd); /* tap interface */

    if (m_tap_fd < 0)
    {
        throw std::runtime_error("Failed starting tap interface.");
    }

    // we'd like to be notified when input is available on stdin
    mainLoop.onReadable(m_tap_fd, [this]() -> bool {
        LOG_DEBUG << "Read from stdin.";
        m_tap.doRead(m_tap_fd);

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
        m_tunTapDriver->setIfUpDown(newIfUp, m_tapName);
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
