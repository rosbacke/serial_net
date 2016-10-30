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

void
TxQueue::sendPacket(const MsgHostIf::HostPkt& data, int address)
{
    const int headerSize = 3;
    ByteVec packet(data.size() + headerSize);

    packet[0] = to_byte(static_cast<uint8_t>(MessageType::send_packet));
    packet[1] = to_byte(static_cast<uint8_t>(address));
    packet[2] = to_byte(static_cast<uint8_t>(m_ownAddress));
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
TxQueue::msgHostTx_sendPacket(const MsgHostIf::HostPkt& data, int srcAddr,
                              int destAddr)
{
    LOG_DEBUG << "Try to send packet from " << srcAddr << " to " << destAddr;
    sendPacket(data, destAddr);
}

void
TxQueue::sendReturnToken()
{
    ByteVec packet(2);

    packet[0] = gsl::to_byte(static_cast<uint8_t>(MessageType::return_token));
    packet[1] = gsl::to_byte(static_cast<uint8_t>(m_ownAddress));
    m_msgEtherIf->sendMsg(packet);
}

void
TxQueue::msgHostTx_sendAddressUpdate(int address, std::array<byte, 6> mac)
{
    const int headerSize = 4;
    ByteVec packet(mac.size() + headerSize);

    packet[0] = to_byte(static_cast<uint8_t>(MessageType::mac_update));
    packet[1] = to_byte(static_cast<uint8_t>(0xff));
    packet[2] = to_byte(static_cast<uint8_t>(m_ownAddress));
    packet[3] = to_byte(static_cast<uint8_t>(address));
    std::copy(mac.begin(), mac.end(), packet.begin() + headerSize);
    m_txMsg.push_back(packet);
}
