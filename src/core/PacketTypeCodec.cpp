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
 * PacketTypeCodec.cpp
 *
 *  Created on: 17 jul 2016
 *      Author: mikaelr
 */

#include "PacketTypeCodec.h"

#include "AddressCache.h"
#include "TxQueue.h"
#include "WSDump.h"
#include "utility/Log.h"

#include <algorithm>
#include <array>
#include <cassert>

using namespace gsl;

class SerialNet;

static bool
longEnough(const MsgEtherIf::EtherPkt& packet)
{
    const auto size = std::size_t(packet.size());
    if (size < 1)
    {
        LOG_WARN << "Packet less than 1 byte.";
        return false;
    }
    auto cmd = byte2MessageType(packet[0]);

#define CHECK_LEN(c, t)  \
    case MessageType::c: \
        return size >= sizeof(packet::t)
    switch (cmd)
    {
        CHECK_LEN(grant_token, GrantToken);
        CHECK_LEN(return_token, ReturnToken);
        CHECK_LEN(master_started, MasterStarted);
        CHECK_LEN(master_ended, MasterEnded);
        CHECK_LEN(send_packet, SendPacket);
        CHECK_LEN(mac_update, MacUpdate);
        CHECK_LEN(address_discovery, AddressDiscovery);
        CHECK_LEN(address_request, AddressRequest);
        CHECK_LEN(address_reply, AddressReply);
    }
#undef CHECK_LEN
    LOG_WARN << "Unknown cmd type." << static_cast<int>(cmd);
    return false;
}

PacketTypeCodec::PacketTypeCodec(MsgEtherIf* msgEtherIf, TxQueue* tx)
    : m_msgEtherIf(msgEtherIf), m_txQueue(tx), m_master(nullptr),
      m_wsDump(nullptr), m_clientAddress(m_txQueue)
{
}

PacketTypeCodec::~PacketTypeCodec()
{
}

void
PacketTypeCodec::rxRawPacket(const MsgEtherIf::EtherPkt& bb)
{
    if (bb.length() < 1)
        return;
    const auto cmd = byte2MessageType(bb[0]);
    if (m_wsDump)
    {
        m_wsDump->rxPacket(bb);
    }
    if (!longEnough(bb))
    {
        LOG_WARN << "Received to short packet, size:" << bb.size() << " cmd "
                 << toString(cmd);
        return;
    }
    LOG_TRACE << "Command: " << toString(cmd) << " raw len: " << bb.size();
    LocalAddress ownAddress = m_txQueue->clientAddress();
    switch (cmd)
    {
    case MessageType::grant_token:
    {
        auto p = packet::toHeader<packet::GrantToken>(bb);
        const LocalAddress msgOwnAddr = p->m_tokenReceiver;
        if (msgOwnAddr == ownAddress)
        {
            m_clientAddress.rxToken(*p);
            m_txQueue->sendClientPacketOrReturnToken();
        }
        break;
    }
    case MessageType::send_packet:
    {
        auto p = packet::toHeader<packet::SendPacket>(bb);
        const LocalAddress srcAddr = p->m_srcAddr;
        const LocalAddress destAddr = p->m_destAddr;
        if ((destAddr == ownAddress && ownAddress != LocalAddress::null_addr) ||
            ownAddress == LocalAddress::broadcast ||
            destAddr == LocalAddress::broadcast)
        {
            LOG_DEBUG << "Got a packet. from: " << srcAddr << " to "
                      << destAddr;
            m_rxMsg.push_back(ByteVec(std::begin(bb), std::end(bb)));
        }
        break;
    }
    case MessageType::address_discovery:
    {
        auto p = packet::toHeader<packet::AddressDiscovery>(bb);
        m_clientAddress.rxDiscoveryPacket(*p);
        break;
    }
    case MessageType::address_request:
        break;

    case MessageType::address_reply:
    {
        auto p = packet::toHeader<packet::AddressReply>(bb);
        m_clientAddress.rxAddrReplyPacket(*p);
        break;
    }
    case MessageType::return_token:
        break;

    case MessageType::master_started:
        m_txQueue->masterStarted();
        break;

    case MessageType::master_ended:
    {
        if (m_masterEndedCB)
        {
            m_masterEndedCB();
        }
        break;
    }
    case MessageType::mac_update:
    {
        if (m_cache)
        {
            const auto localAddr = toLocalAddress(bb[3]);
            std::array<byte, 6> mac;
            std::copy(bb.data() + 4, bb.data() + 10, mac.begin());
            m_cache->setAddress(localAddr, mac);
        }
        break;
    }
    }
    if (m_master)
    {
        m_master->masterPacketReceived(cmd, bb);
    }
    deliverRxQueue();
}

void
PacketTypeCodec::deliverRxQueue()
{
    while (!m_rxMsg.empty())
    {
        const auto& f = m_rxMsg.front();
        MsgEtherIf::EtherPkt bb(f.data(), f.size());
        assert(bb.size() >= (int)sizeof(packet::SendPacket));
        auto p = packet::toHeader<packet::SendPacket>(bb);
        LOG_DEBUG << "Rx packet with size: " << bb.size()
                  << " from: " << p->m_srcAddr
                  << " type: " << toString(p->m_chType);

        auto res = ByteVec(bb.begin() + sizeof(packet::SendPacket), bb.end());
        auto rxIf = m_txQueue->getRxIf(p->m_chType);
        if (rxIf)
        {
            rxIf->packetReceivedFromNet(res, p->m_srcAddr, p->m_destAddr,
                                        p->m_chType);
        }
        m_rxMsg.pop_front();
    }
}

void
PacketTypeCodec::msgEtherRx_newMsg(const MsgEtherIf::EtherPkt& packet)
{
    // LOG_DEBUG << "Got packet from MsgEther interface.";
    rxRawPacket(packet);
}
