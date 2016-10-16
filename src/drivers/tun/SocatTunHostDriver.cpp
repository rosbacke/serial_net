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

#include "SocatTunHostDriver.h"
#include "utility/Log.h"

SocatTunHostDriver::SocatTunHostDriver(int myAddr)
    : m_myAddr(myAddr), m_txIf(nullptr)
{
}

SocatTunHostDriver::~SocatTunHostDriver()
{
}

void
SocatTunHostDriver::startTransfer(MsgHostIf::TxIf* txIf, React::Loop& loop)
{
    m_txIf = txIf;
    setupCallback(loop);
}

namespace
{
// Header including IPv4

struct Ipv4Header
{
    uint8_t m_flags[2];
    uint8_t m_length[2];
    uint8_t m_identifier[2];
    uint8_t m_flagsOffset[2];
    uint8_t m_ttl[1];
    uint8_t m_protocol[1];
    uint8_t m_checksum[2];
    uint8_t m_srcAddr[4];
    uint8_t m_destAddr[4];

    int getLength() const
    {
        return m_length[0] * 0x100 + m_length[1];
    }
};

struct TunIpv4Header
{
    uint8_t m_tun[4];
    Ipv4Header m_ipv4;
};
}

void
SocatTunHostDriver::doRead(int fd)
{
    int readlen;
    int srcAddr = 0;
    int destAddr = 0;
    switch (m_readType)
    {
    case ReadType::header:
    {
        readlen = ::read(fd, m_readHeader.data(), headerLen);
        if (readlen != headerLen)
        {
            LOG_INFO << "Unexpected read len in header, len:" << readlen;
            throw std::runtime_error("Unexpected read length");
        }
        TunIpv4Header* tunHeader =
            reinterpret_cast<TunIpv4Header*>(m_readHeader.data());
        Ipv4Header* ipv4Header = &tunHeader->m_ipv4;
        int length = ipv4Header->getLength();
        LOG_DEBUG << "Tun IP length: " << length;

        m_rxTunPacket.resize(length + sizeof(TunIpv4Header::m_tun));
        // Skip the TUN header, include full IP header.
        std::transform(m_readHeader.begin(), m_readHeader.end(),
                       m_rxTunPacket.begin(),
                       [](auto el) { return gsl::to_byte(el); });
        m_readType = ReadType::data;
        break;
    }
    case ReadType::data:
    {
        const TunIpv4Header* tunIpv4Header =
            reinterpret_cast<TunIpv4Header*>(m_rxTunPacket.data());
        const Ipv4Header* ipv4Header = &tunIpv4Header->m_ipv4;

        int readMaxLen = m_rxTunPacket.size() - sizeof(TunIpv4Header);
        srcAddr = ipv4Header->m_srcAddr[3];
        destAddr = ipv4Header->m_destAddr[3];

        void* start = m_rxTunPacket.data() + sizeof(TunIpv4Header);
        readlen = ::read(fd, start, readMaxLen);
        m_readType = ReadType::header;
        if (readlen != readMaxLen)
        {
            LOG_INFO << "Unexpected read len in data, exp len: " << readMaxLen
                     << " actual: " << readlen;
            throw std::runtime_error("Unexpected read length");
        }

        if (m_txIf)
        {
            m_txIf->msgHostTx_sendPacket(m_rxTunPacket, srcAddr, destAddr);
        }
        break;
    }
    }
}

void
SocatTunHostDriver::setupCallback(React::Loop& mainLoop)
{
    // we'd like to be notified when input is available on stdin
    mainLoop.onReadable(STDIN_FILENO, [this]() -> bool {
        LOG_DEBUG << "Read from stdin.";
        this->doRead(STDIN_FILENO);

        // return true, so that we also return future read events
        return true;
    });
}

/**
 * Called when a packet was received from the serial net.
 */
void
SocatTunHostDriver::packetReceived(const ByteVec& data, int srcAddr,
                                   int destAddr)
{
    int writeLen = ::write(STDOUT_FILENO, data.data(), data.size());
    if (writeLen != (int)data.size())
    {
        LOG_INFO << "Unexpected write len in data, data:" << writeLen;
        throw std::runtime_error("Unexpected write length");
    }
    LOG_INFO << "Write packet to TUN, len " << writeLen;
}
