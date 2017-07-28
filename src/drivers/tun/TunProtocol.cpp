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
 * TunProtocol.cpp
 *
 *  Created on: 23 juli 2017
 *      Author: mikaelr
 */

#include "TunProtocol.h"

#include "hal/PosixIf.h"
#include "interfaces/MsgHostIf.h"
#include "utility/Log.h"

using namespace gsl;

TunProtocol::~TunProtocol()
{
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
TunProtocol::doRead(int fd)
{
    int readlen;
    LocalAddress destAddr = LocalAddress::null_addr;
    switch (m_readType)
    {
    case ReadType::header:
    {
        readlen = m_posixFileIf->read(fd, m_readHeader.data(), headerLen);
        LOG_DEBUG << "TUN header read len:" << readlen;
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
        destAddr = toLocalAddress(to_byte(ipv4Header->m_destAddr[3]));

        void* start = m_rxTunPacket.data() + sizeof(TunIpv4Header);
        readlen = m_posixFileIf->read(fd, start, readMaxLen);
        LOG_DEBUG << "TUN data read len:" << readlen;
        m_readType = ReadType::header;
        if (readlen != readMaxLen)
        {
            LOG_INFO << "Unexpected read len in data, exp len: " << readMaxLen
                     << " actual: " << readlen;
            throw std::runtime_error("Unexpected read length");
        }

        if (m_txIf)
        {
            MsgHostIf::HostPkt hostPkt(m_rxTunPacket.data(),
                                       m_rxTunPacket.size());
            m_txIf->msgHostTx_sendPacket(hostPkt, destAddr,
                                         ChannelType::tun_format);
        }
        break;
    }
    }
}

void
TunProtocol::doRead2(int fd)
{
    LocalAddress destAddr = LocalAddress::null_addr;

    const int buffLen = 2048;
    std::array<byte, buffLen> readBuf;
    int readlen = m_posixFileIf->read(fd, readBuf.data(), buffLen);

    LOG_DEBUG << "TUN header read len:" << readlen;
    if (readlen < headerLen)
    {
        LOG_INFO << "Unexpected read len in header, len:" << readlen;
        throw std::runtime_error("Unexpected read length");
    }

    TunIpv4Header* tunHeader = reinterpret_cast<TunIpv4Header*>(readBuf.data());
    Ipv4Header* ipv4Header = &tunHeader->m_ipv4;
    int length = ipv4Header->getLength();
    LOG_DEBUG << "Tun IP length: " << length;
    destAddr = toLocalAddress(to_byte(ipv4Header->m_destAddr[3]));

    if (m_txIf)
    {
        MsgHostIf::HostPkt hostPkt(readBuf.data(),
                                   length + sizeof(TunIpv4Header::m_tun));
        m_txIf->msgHostTx_sendPacket(hostPkt, destAddr,
                                     ChannelType::tun_format);
    }
}

/**
 * Called when a packet was received from the serial net.
 */
void
TunProtocol::packetReceivedFromNet(int fd, const ByteVec& data,
                                   LocalAddress srcAddr, LocalAddress destAddr)
{
    int writeLen = m_posixFileIf->write(fd, data.data(), data.size());
    if (writeLen != (int)data.size())
    {
        LOG_INFO << "Unexpected write len in data, data:" << writeLen;
        throw std::runtime_error("Unexpected write length");
    }
    LOG_INFO << "Write packet to TUN, len " << writeLen;
}
