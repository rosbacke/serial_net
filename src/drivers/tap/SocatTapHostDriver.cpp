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
 *  Created on: 21 sep. 2016
 *      Author: mikaelr
 */

#include "SocatTapHostDriver.h"

#include "utility/Log.h"

#include <array>
#include <unistd.h>

using std::array;
using namespace gsl;

SocatTapHostDriver::SocatTapHostDriver(AddressCache* ac, PosixFileIf* pfi)
    : m_tap(ac, pfi)
{
}

SocatTapHostDriver::~SocatTapHostDriver()
{
}

void
SocatTapHostDriver::packetReceived(const ByteVec& data, LocalAddress srcAddr,
                                   LocalAddress destAddr)
{
    m_tap.packetReceived(STDOUT_FILENO, data, srcAddr, destAddr);
}

void
SocatTapHostDriver::startTransfer(MsgHostIf* txIf, EventLoop& loop)
{
    m_tap.setTx(txIf);
    setupCallback(loop);
    txIf->setRxHandler(this);
}

void
SocatTapHostDriver::setupCallback(EventLoop& mainLoop)
{
    // we'd like to be notified when input is available on stdin
    mainLoop.onReadable(STDIN_FILENO, [this]() -> bool {
        LOG_DEBUG << "Read from stdin.";
        m_tap.doRead(STDIN_FILENO);

        // return true, so that we also return future read events
        return true;
    });
}
