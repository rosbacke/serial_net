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
TxQueue::sendPacket(const MsgHostIf::HostPkt& data, LocalAddress address)
{
    using PktType = packet::SendPacket;
    auto headerSize = sizeof(PktType);
    ByteVec packet(data.size() + headerSize);
    auto p = toHeader<PktType>(packet.data());

    p->m_type = MessageType::send_packet;
    p->m_destAddr = address;
    p->m_srcAddr = m_ownAddress;
    std::copy(data.begin(), data.end(), packet.begin() + headerSize);

    m_txMsg.push_back(packet);
}

bool
TxQueue::sendClientPacket(bool useReturnToken)
{
    if (m_txMsg.empty())
    {
        if (useReturnToken)
        {
            sendReturnToken();
        }
        LOG_DEBUG << "Nothing to send from own client.";
        return false;
    }
    auto packet = m_txMsg.front();
    m_msgEtherIf->sendMsg(packet);
    m_txMsg.pop_front();
    return true;
}

void
TxQueue::sendMasterPacket(const ByteVec& packet)
{
    m_msgEtherIf->sendMsg(packet);
}

void
TxQueue::msgHostTx_sendPacket(const MsgHostIf::HostPkt& data,
                              LocalAddress srcAddr, LocalAddress destAddr)
{
    LOG_DEBUG << "Try to send packet from " << srcAddr << " to " << destAddr;
    sendPacket(data, destAddr);
}

void
TxQueue::sendReturnToken()
{
    ByteVec packet(2);
    auto p = toHeader<packet::ReturnToken>(packet.data());

    p->m_type = MessageType::return_token;
    p->m_src = m_ownAddress;
    m_msgEtherIf->sendMsg(packet);
}

void
TxQueue::msgHostTx_sendAddressUpdate(LocalAddress address,
                                     std::array<byte, 6> mac)
{
    using PktType = packet::MacUpdate;
    auto headerSize = sizeof(PktType);
    ByteVec packet(headerSize);
    auto p = toHeader<PktType>(packet.data());

    p->m_type = MessageType::mac_update;
    p->m_destAddr = LocalAddress::broadcast;
    p->m_srcAddr = m_ownAddress;
    p->m_hintAddr = address;
    std::copy(mac.begin(), mac.end(), p->m_mac.data());

    m_txMsg.push_back(packet);
}
