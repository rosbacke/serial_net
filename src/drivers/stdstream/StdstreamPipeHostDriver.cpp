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
 * StdstreamOpipeHostDriver.cpp
 *
 *  Created on: 14 aug. 2016
 *      Author: mikaelr
 */

#include "StdstreamPipeHostDriver.h"

#include "utility/Log.h"

StdstreamPipeHostDriver::StdstreamPipeHostDriver(int myAddr)
    : m_myAddr(myAddr), m_rxAddr(-1), m_destAddr(-1), m_txHandler(nullptr)
{
}

StdstreamPipeHostDriver::~StdstreamPipeHostDriver()
{
}

void
StdstreamPipeHostDriver::startStdout(int rxAddress)
{
    m_rxAddr = rxAddress;
}

void
StdstreamPipeHostDriver::startStdin(int destAddr, TxIf* txIf,
                                    React::MainLoop& mainLoop)
{
    m_destAddr = destAddr;
    setTxHandler(txIf);
    setupCallback(mainLoop);
}

void
StdstreamPipeHostDriver::setupCallback(React::MainLoop& mainLoop)
{
    // we'd like to be notified when input is available on stdin
    mainLoop.onReadable(STDIN_FILENO, [this]() -> bool {
        // Enough to keep an ethernet frame.
        const constexpr int maxRead = 2000;
        gsl::byte buffer[maxRead];
        ByteVec buf;
        ssize_t dataRead;
        dataRead = ::read(STDIN_FILENO, buffer, maxRead);
        if (dataRead > 0)
        {
            buf.resize(dataRead);
            std::transform(buffer, buffer + dataRead, buf.begin(),
                           [](const auto& el) -> gsl::byte {
                               return gsl::to_byte(
                                   static_cast<unsigned char>(el));
                           });
            MsgHostIf::HostPkt hostPkt(static_cast<const gsl::byte*>(buffer),
                                       dataRead);
            if (m_txHandler && m_destAddr > 0)
            {
                m_txHandler->msgHostTx_sendPacket(hostPkt, m_myAddr,
                                                  m_destAddr);
            }
        }
        // return true, so that we also return future read events
        return true;
    });
}

void
StdstreamPipeHostDriver::packetReceived(const ByteVec& data, int srcAddr,
                                        int destAddr)
{

    std::transform(data.begin(), data.end(),
                   std::ostream_iterator<uint8_t>(std::cout),
                   [](gsl::byte el) { return gsl::to_integer<char>(el); });
    std::cout.flush();
}
