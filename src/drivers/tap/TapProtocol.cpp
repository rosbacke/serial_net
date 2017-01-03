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
 * TapProtocol.cpp
 *
 *  Created on: 27 okt. 2016
 *      Author: mikaelr
 */

#include "TapProtocol.h"

#include "hal/PosixIf.h"
#include "utility/Log.h"
#include <array>
#include <unistd.h>

using std::array;
using namespace gsl;

struct TapProtocol::EtherHeader
{
    array<byte, 6> m_destMAC;
    array<byte, 6> m_srcMAC;
    array<byte, 2> m_type;
};

struct TapProtocol::EtherFooter
{
    array<uint8_t, 4> m_crc;
};

struct TapProtocol::ArpIpv4Header
{
    array<byte, 2> m_htype;
    array<byte, 2> m_ptype;
    byte m_hLen;
    byte m_pLen;
    array<byte, 2> m_oper;
    array<byte, 6> m_SHA;
    array<byte, 4> m_SPA;
    array<byte, 6> m_THA;
    array<byte, 4> m_TPA;
};

// Header including IPv4
struct TapProtocol::Ipv4Header
{
    byte m_flags[2];
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

struct TapProtocol::TapHeader
{
    uint8_t m_tap[4];
    EtherHeader m_ether;
    union {
        Ipv4Header m_ipv4;
        ArpIpv4Header m_arpIpv4;
    };
};

void
TapProtocol::doRead(int fd)
{
    int readLen;
    LocalAddress destAddr = LocalAddress::null_addr;

    std::vector<byte> rx;

    // Havn't got the length. Assume we get a full packet when reading
    // from the TAP device. Assume 1500 MTU. Add some margin.
    constexpr size_t bufSize = 1600;

    rx.resize(bufSize);
    readLen = m_posixFileIf->read(fd, rx.data(), bufSize);
    rx.resize(readLen);
    TapHeader* tapHeader = reinterpret_cast<TapHeader*>(rx.data());

    LOG_DEBUG << "Read TAP total length: " << readLen;

    EtherHeader* etherHeader = &tapHeader->m_ether;
    m_cache->updateMyMAC(m_txIf->msgHostTx_clientAddress(),
                         etherHeader->m_srcMAC);

    destAddr = m_cache->getLocalAddress(tapHeader->m_ether.m_destMAC);
    if (destAddr == LocalAddress::null_addr)
    {
        LOG_INFO << "Tap: no local address for mac. Skip tx.";
        return;
    }

    // Possibly send out updates of the ethernet address.
    checkArp(tapHeader);

    if (m_txIf)
    {
        MsgHostIf::HostPkt hostPkt(rx.data(), readLen);
        m_txIf->msgHostTx_sendPacket(hostPkt, destAddr);
    }
}

/**
 * Called when a packet was received from the serial net.
 */
void
TapProtocol::packetReceived(int fd, const ByteVec& data, LocalAddress srcAddr,
                            LocalAddress destAddr)
{
    LOG_DEBUG << "Try packet write, packetReceived.";
    if (m_cache == nullptr)
    {
        LOG_DEBUG << "No cache.";
        return;
    }
    if (srcAddr != LocalAddress::broadcast)
    {
        auto srcMac = m_cache->getMac(srcAddr);
        if (!srcMac.first)
        {
            const TapHeader* tapHeader =
                reinterpret_cast<const TapHeader*>(data.data());
            m_cache->setAddress(srcAddr, tapHeader->m_ether.m_srcMAC);
        }
    }
    auto srcMac = m_cache->getMac(srcAddr);
    auto destMac = m_cache->getMac(destAddr);
    if (!srcMac.first || !destMac.first)
    {
        LOG_DEBUG << "Addr err, src" << !srcMac.first << " dest "
                  << !destMac.first;
        return;
    }
    int writeLen = m_posixFileIf->write(fd, data.data(), data.size());
    if (writeLen != (int)data.size())
    {
        LOG_INFO << "Unexpected write len in data, data:" << writeLen;
        throw std::runtime_error("Unexpected write length");
    }
    LOG_INFO << "Write packet to TAP, len " << writeLen;
}

// Check for an ARP. If found broadcast our MAC to allow serial_net to map
// local addresses to MAC addresses.
void
TapProtocol::checkArp(const TapHeader* tap)
{
    if (m_txIf == nullptr)
    {
        return;
    }
    auto ethArpType = array<byte, 2>{{to_byte<0x08>(), to_byte<0x06>()}};
    if (tap->m_ether.m_type == ethArpType)
    {
        const ArpIpv4Header& arp = tap->m_arpIpv4;
        auto ptypeIpv4 = array<byte, 2>{{to_byte<0x08>(), to_byte<0x00>()}};
        auto operReq = array<byte, 2>{{to_byte<0x00>(), to_byte<0x01>()}};
        auto operReply = array<byte, 2>{{to_byte<0x00>(), to_byte<0x02>()}};
        if (arp.m_ptype == ptypeIpv4 &&
            (arp.m_oper == operReply || arp.m_oper == operReq))
        {
            // Pick up the ethernet addr from the layer 2. Simulate existing
            // practice
            // in layer 2 switches.
            m_txIf->msgHostTx_sendMacUpdate(tap->m_ether.m_srcMAC);
        }
    }
}
