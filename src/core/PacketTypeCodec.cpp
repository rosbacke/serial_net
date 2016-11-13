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
    }
#undef CHECK_LEN
    LOG_WARN << "Unknown cmd type." << static_cast<int>(cmd);
    return false;
}

PacketTypeCodec::PacketTypeCodec(MsgEtherIf* msgEtherIf, TxQueue* tx,
                                 LocalAddress ownAddress)
    : m_msgEtherIf(msgEtherIf), m_msgHostIf(nullptr), m_txQueue(tx),
      m_master(nullptr), m_wsDump(nullptr), m_ownAddress(ownAddress)
{
}

PacketTypeCodec::~PacketTypeCodec()
{
}

void
PacketTypeCodec::rxRawPacket(const MsgEtherIf::EtherPkt& bb)
{
    if (!longEnough(bb))
    {
        return;
    }
    if (m_wsDump)
    {
        m_wsDump->rxPacket(bb);
    }
    const auto cmd = byte2MessageType(bb[0]);
    LOG_TRACE << "Command: " << cmd << " raw len: " << bb.size();

    switch (cmd)
    {
    case MessageType::grant_token:
    {
        const LocalAddress msgOwnAddr = toLocalAddress(bb[1]);
        if (msgOwnAddr == m_ownAddress && m_txQueue)
        {
            m_txQueue->sendClientPacketOrReturnToken();
        }
        break;
    }
    case MessageType::send_packet:
    {
        assert(bb.size() >= 3);
        const LocalAddress destAddr = toLocalAddress(bb[1]);
        // uint8_t srcAddr = bb[2];
        if (destAddr == m_ownAddress ||
            m_ownAddress == LocalAddress::broadcast ||
            destAddr == LocalAddress::broadcast)
        {
            LOG_DEBUG << "Got a packet from: " << destAddr;
            m_rxMsg.push_back(ByteVec(std::begin(bb), std::end(bb)));
        }
        break;
    }
    case MessageType::return_token:
        break;
    case MessageType::master_started:
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

    default:
        break;
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
        const int headerSize = 3;
        // ByteBuf bb(m_rxMsg.front());
        const auto& f = m_rxMsg.front();
        MsgEtherIf::EtherPkt bb(f.data(), f.size());
        LOG_DEBUG << "Deliver packet with size: " << bb.size();
        assert(bb.size() >= 3);
        auto destAddr = toLocalAddress(bb[1]);
        auto srcAddr = toLocalAddress(bb[2]);
        auto res = ByteVec(bb.begin() + headerSize, bb.end());
        if (m_msgHostIf)
        {
            m_msgHostIf->packetReceived(res, srcAddr, destAddr);
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
