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
#include "WSDump.h"
#include "utility/Log.h"

#include <cassert>

using namespace gsl;

class SerialNet;

static bool
longEnough(const std::vector<gsl::byte>& packet)
{
    const auto size = packet.size();
    if (size < 1)
    {
        LOG_WARN << "Packet less than 1 byte.";
        return false;
    }
    const auto cmd = static_cast<MessageType>(packet[0]);

    using Entry = std::pair<MessageType, std::size_t>;
    static Entry minLength[] = {
        {MessageType::grant_token, std::size_t(FrameGrantToken::max_num)},
        {MessageType::return_token, std::size_t(FrameReturnToken::max_num)},
        {MessageType::master_started, 1},
        {MessageType::master_ended, 1},
        {MessageType::send_packet, std::size_t(FrameSendPacket::packet_data)},
        {MessageType::send_tun_packet, 1},
        {MessageType::mac_update, 10},
        {MessageType::lookup_address, 1}};

    const auto end = minLength + ARRAY_SIZE(minLength);
    const auto iter = std::find_if(
        minLength, end, [cmd](const Entry& el) { return cmd == el.first; });
    if (iter == end)
    {
        LOG_WARN << "Unknown cmd type." << static_cast<int>(cmd);
        return false;
    }
    const bool ret = size >= iter->second;
    LOG_TRACE << "packet len check status: " << ret;
    return ret;
}

PacketTypeCodec::PacketTypeCodec(MsgEtherIf* msgEtherIf, int ownAddress)
    : m_msgEtherIf(msgEtherIf), m_msgHostIf(nullptr), m_master(nullptr),
      m_wsDump(nullptr), m_ownAddress(ownAddress)
{
}

PacketTypeCodec::~PacketTypeCodec()
{
}

void
PacketTypeCodec::rxRawPacket(const ByteBuf& bb)
{
    if (!longEnough(bb.get()))
    {
        return;
    }
    if (m_wsDump)
    {
        m_wsDump->rxPacket(bb.get());
    }
    auto cmd = static_cast<MessageType>(bb[0]);
    LOG_TRACE << "Command: " << int(cmd) << " raw len: " << bb.size();

    switch (cmd)
    {
    case MessageType::grant_token:
    {
        const int msgOwnAddr = gsl::to_integer<int>(bb[1]);
        if (msgOwnAddr == m_ownAddress)
        {
            if (!sendClientPacket())
            {
                sendReturnToken();
            }
        }
        break;
    }
    case MessageType::send_packet:
    {
        assert(bb.get().size() >= 3);
        const int destAddr = gsl::to_integer<int>(bb[1]);
        // uint8_t srcAddr = bb[2];
        if (destAddr == m_ownAddress || m_ownAddress == 255 || destAddr == 255)
        {
            LOG_DEBUG << "Got a packet from: " << destAddr;
            m_rxMsg.push_back(bb);
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
            const int localAddr = gsl::to_integer<int>(bb[3]);
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
        m_master->masterPacketReceived(cmd, bb.get());
    }
    deliverRxQueue();
}

void
PacketTypeCodec::deliverRxQueue()
{
    while (!m_rxMsg.empty())
    {
        const int headerSize = 3;
        ByteBuf bb(m_rxMsg.front());
        LOG_DEBUG << "Deliver packet with size: " << bb.get().size();
        assert(bb.get().size() >= 3);
        auto destAddr = gsl::to_integer<int>(bb[1]);
        auto srcAddr = gsl::to_integer<int>(bb[2]);
        auto res = ByteVec(bb.get().begin() + headerSize, bb.get().end());
        m_rxMsg.pop_front();
        if (m_msgHostIf)
        {
            m_msgHostIf->packetReceived(res, srcAddr, destAddr);
        }
    }
}

void
PacketTypeCodec::sendReturnToken()
{
    ByteVec packet(2);

    packet[0] = gsl::to_byte(static_cast<uint8_t>(MessageType::return_token));
    packet[1] = gsl::to_byte(static_cast<uint8_t>(m_ownAddress));
    m_msgEtherIf->sendMsg(packet);
}

void
PacketTypeCodec::msgEtherRx_newMsg(const ByteVec& msg)
{
    // LOG_DEBUG << "Got packet from MsgEther interface.";
    rxRawPacket(ByteBuf(msg));
}

void
PacketTypeCodec::msgHostTx_sendAddressUpdate(int address,
                                             std::array<byte, 6> mac)
{
    const int headerSize = 4;
    ByteVec packet(mac.size() + headerSize);

    packet[0] = to_byte(static_cast<uint8_t>(MessageType::mac_update));
    packet[1] = to_byte(static_cast<uint8_t>(0xff));
    packet[2] = to_byte(static_cast<uint8_t>(m_ownAddress));
    packet[3] = to_byte(static_cast<uint8_t>(address));
    std::copy(mac.begin(), mac.end(), packet.begin() + headerSize);
    m_txMsg.push_back(ByteBuf(packet));
}

void
PacketTypeCodec::sendPacket(const ByteVec& data, int address)
{
    const int headerSize = 3;
    ByteVec packet(data.size() + headerSize);

    packet[0] = to_byte(static_cast<uint8_t>(MessageType::send_packet));
    packet[1] = to_byte(static_cast<uint8_t>(address));
    packet[2] = to_byte(static_cast<uint8_t>(m_ownAddress));
    std::copy(data.begin(), data.end(), packet.begin() + headerSize);

    m_txMsg.push_back(ByteBuf(packet));
}

bool
PacketTypeCodec::sendClientPacket()
{
    if (m_txMsg.empty())
    {
        return false;
    }
    auto packet = m_txMsg.front();
    m_msgEtherIf->sendMsg(packet.get());
    m_txMsg.pop_front();
    return true;
}

void
PacketTypeCodec::sendMasterPacket(const ByteVec& packet)
{
    m_msgEtherIf->sendMsg(packet);
}

void
PacketTypeCodec::msgHostTx_sendPacket(const ByteVec& data, int srcAddr,
                                      int destAddr)
{
    LOG_DEBUG << "Try to send packet from " << srcAddr << " to " << destAddr;
    sendPacket(data, destAddr);
}
