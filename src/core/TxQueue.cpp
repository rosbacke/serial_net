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
 * TxQueue.cpp
 *
 *  Created on: 30 okt. 2016
 *      Author: mikaelr
 */

#include "TxQueue.h"
#include "gsl/gsl"

#include "utility/Log.h"

using gsl::to_byte;
using gsl::byte;
using packet::toHeader;

void
TxQueue::sendPacket(const MsgHostIf::HostPkt& data, LocalAddress destAddress,
                    ChannelType chType)
{
    LOG_DEBUG << "TxQueue, send packet:" << data.size();
    using PktType = packet::SendPacket;
    auto headerSize = sizeof(PktType);
    ByteVec packet(data.size() + headerSize);
    auto p = toHeader<PktType>(packet.data());

    p->m_type = MessageType::send_packet;
    p->m_chType = chType;
    p->m_destAddr = destAddress;
    p->m_srcAddr = m_ownAddress.addr();
    std::copy(data.begin(), data.end(), packet.begin() + headerSize);

    m_txMsg.push_back(packet);
}

void
TxQueue::sendClientPacketOrReturnToken()
{
    if (m_txMsg.empty())
    {
        sendReturnToken();
        LOG_DEBUG << "Send return token. tx queue empty.";
    }
    else
    {
        sendClientPacket();
    }
}

void
TxQueue::sendClientPacket()
{
    auto packet = m_txMsg.front();

    m_msgEtherIf->sendMsg(MsgEtherIf::EtherPkt(packet));
    m_txMsg.pop_front();
}

void
TxQueue::sendMasterPacket(const MsgEtherIf::EtherPkt& packet)
{
    m_msgEtherIf->sendMsg(packet);
}

void
TxQueue::msgHostTx_sendPacket(const MsgHostIf::HostPkt& data,
                              LocalAddress destAddr, ChannelType chType)
{
    if (m_ownAddress.valid())
    {
        LOG_DEBUG << "Try to send packet from " << m_ownAddress.addr() << " to "
                  << destAddr;
        sendPacket(data, destAddr, chType);
    }
}

void
TxQueue::sendReturnToken()
{
    packet::ReturnToken p;
    p.m_type = MessageType::return_token;
    p.m_src = m_ownAddress.addr();
    m_msgEtherIf->sendMsg(packet::fromHeader(p));
}

void TxQueue::msgHostTx_sendMacUpdate(std::array<byte, 6> mac)
{
    if (!m_ownAddress.valid())
    {
        return;
    }
    using PktType = packet::MacUpdate;
    auto headerSize = sizeof(PktType);
    ByteVec packet(headerSize);
    auto p = toHeader<PktType>(packet.data());

    p->m_type = MessageType::mac_update;
    p->m_destAddr = LocalAddress::broadcast;
    p->m_srcAddr = m_ownAddress.addr();
    p->m_hintAddr = m_ownAddress.addr();
    std::copy(mac.begin(), mac.end(), p->m_mac.data());

    m_txMsg.push_back(packet);
}

void
TxQueue::masterStarted()
{
    m_ownAddress.masterStarted();
}

void
TxQueue::setRxHandler(RxIf* rxIf, ChannelType chType)
{
    switch (chType)
    {
    case ChannelType::raw_stream:
        m_rxIfRaw = rxIf;
        return;
    case ChannelType::tap_format:
        m_rxIfTap = rxIf;
        return;
    case ChannelType::tun_format:
        m_rxIfTun = rxIf;
        return;
    case ChannelType::illegal_type:
        return;
    }
}

MsgHostIf::RxIf*
TxQueue::getRxIf(ChannelType chType) const
{
    switch (chType)
    {
    case ChannelType::raw_stream:
        return m_rxIfRaw;
    case ChannelType::tap_format:
        return m_rxIfTap;
    case ChannelType::tun_format:
        return m_rxIfTun;
    case ChannelType::illegal_type:
        return nullptr;
    }
    return nullptr;
}
