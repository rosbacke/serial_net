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

using std::array;
using namespace gsl;

SocatTapHostDriver::SocatTapHostDriver(int myAddr) : m_myAddr(myAddr)
{
}

SocatTapHostDriver::~SocatTapHostDriver()
{
}

void
SocatTapHostDriver::startTransfer(MsgHostIf::TxIf* txIf, React::Loop& loop)
{
    m_txIf = txIf;
    setupCallback(loop);
}

struct SocatTapHostDriver::EtherHeader
{
    array<byte, 6> m_destMAC;
    array<byte, 6> m_srcMAC;
    array<byte, 2> m_type;
};

struct SocatTapHostDriver::EtherFooter
{
    array<uint8_t, 4> m_crc;
};

struct SocatTapHostDriver::ArpIpv4Header
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
struct SocatTapHostDriver::Ipv4Header
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

struct SocatTapHostDriver::TapHeader
{
    uint8_t m_tap[4];
    EtherHeader m_ether;
    union {
        Ipv4Header m_ipv4;
        ArpIpv4Header m_arpIpv4;
    };
};

void
SocatTapHostDriver::doRead(int fd)
{
    int readLen;
    int destAddr = 0;

    std::vector<byte> rx;

    // Havn't got the length. Assume we get a full packet when reading
    // from the TAP device. Assume 1500 MTU. Add some margin.
    constexpr size_t bufSize = 1600;

    rx.resize(bufSize);
    readLen = ::read(fd, rx.data(), bufSize);
    rx.resize(readLen);
    TapHeader* tapHeader = reinterpret_cast<TapHeader*>(rx.data());

    LOG_DEBUG << "Read TAP total length: " << readLen;

    EtherHeader* etherHeader = &tapHeader->m_ether;
    m_cache->updateMyMAC(m_myAddr, etherHeader->m_srcMAC);

    destAddr = m_cache->getLocalAddress(tapHeader->m_ether.m_destMAC);
    if (destAddr == -1)
    {
        LOG_DEBUG << "Tap: no local address for mac. Skip tx.";
        return;
    }

    // Possibly send out updates of the ethernet address.
    checkArp(tapHeader);

    if (m_txIf)
    {
        m_txIf->msgHostTx_sendPacket(rx, m_myAddr, destAddr);
    }
}

void
SocatTapHostDriver::setupCallback(React::Loop& mainLoop)
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
SocatTapHostDriver::packetReceived(const ByteVec& data, int srcAddr,
                                   int destAddr)
{
    if (m_cache == nullptr)
    {
        return;
    }
    auto srcMac = m_cache->getMac(srcAddr);
    auto destMac = m_cache->getMac(destAddr);
    if (!srcMac.first || !destMac.first)
    {
        return;
    }
    int writeLen = ::write(STDOUT_FILENO, data.data(), data.size());
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
SocatTapHostDriver::checkArp(const TapHeader* tap)
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
            m_txIf->msgHostTx_sendAddressUpdate(m_myAddr,
                                                tap->m_ether.m_srcMAC);
        }
    }
}
