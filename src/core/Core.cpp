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
 * Core.cpp
 *
 *  Created on: 1 jul 2016
 *      Author: mikaelr
 */

#include "Core.h"
#include "SerialProtocol.h"
#include "utility/Utility.h"

namespace
{
template <class T, class U>
void
setValue(ByteVec& bv, T member, U arg)
{
    bv[static_cast<std::size_t>(member)] =
        gsl::to_byte(static_cast<uint8_t>(arg));
}

#if 0
template<enum T, class U>
void setCValue(ByteVec& bv)
{
	bv[ static_cast<std::size_t>(T)] = gsl::to_byte<U>();
}
#endif
}

Core::Core(ByteStreamIf* bsif, DecodeCB clientCB, void* cbData)
    : m_bsif(bsif), m_frameCodec(this), m_clientCB(clientCB),
      m_clientCBData(cbData)
{
}

/**
 * Receive a decoded packet from the serial net.
 */
void
Core::receiveSerialPacket(const ByteVec& packet)
{
    if (m_clientCB)
    {
        m_clientCB(packet, m_clientCBData);
    }
}

void
Core::sendPacketToSerial(int destAddr, int tokenHint, const ByteVec& data)
{
    ByteVec packet;

    const std::size_t packet_offset =
        static_cast<std::size_t>(FrameSendPacket::packet_data);
    packet.resize(data.size() + packet_offset);

    setValue(packet, FrameSendPacket::msg_type, MessageType::send_packet);

    setValue(packet, FrameSendPacket::dest_addr, destAddr);
    setValue(packet, FrameSendPacket::token_hint, tokenHint);

    // packet[ static_cast<std::size_t>(FrameSendPacket::msg_type) ] =
    // gsl::to_byte<int>(MessageType::send_packet);
    // packet[ static_cast<std::size_t>(FrameSendPacket::dest_addr) ] =
    // destAddr;
    // packet[ static_cast<std::size_t>(FrameSendPacket::token_hint) ] =
    // tokenHint;
    std::copy(data.begin(), data.end(), packet.begin() + packet_offset);

    ByteVec packetOut;
    m_frameCodec.encodePacket(packet, packetOut);
    m_bsif->write(data);
}
