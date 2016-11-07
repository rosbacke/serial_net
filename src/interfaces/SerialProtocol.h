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
 * SerialProtocol.h
 *
 *  Created on: 2 jul 2016
 *      Author: mikaelr
 */

#ifndef SRC_INTERFACES_SERIALPROTOCOL_H_
#define SRC_INTERFACES_SERIALPROTOCOL_H_

#include "gsl/gsl"
#include <array>

/**
 * Defined message type for the system.
 */
enum class MessageType : uint8_t
{
    grant_token,
    return_token,
    master_started,
    master_ended,
    send_packet,
    mac_update,
};

inline gsl::byte
messageType2Byte(MessageType msgType)
{
    return gsl::to_byte(static_cast<uint8_t>(msgType));
}

inline MessageType
byte2MessageType(gsl::byte b)
{
    return static_cast<MessageType>(b);
}

/**
 * Address allocations:
 * 1 byte address between 0-255.
 * Address:
 * 0 : unallocated node, master, invalid address.
 * 1-254 possible client addresses.
 * 255 broadcast.
 */
enum class LocalAddress : uint8_t
{
    null_addr = 0,
    broadcast = 0xff
};

inline gsl::byte
localAddress2Byte(LocalAddress local)
{
    return gsl::to_byte(static_cast<uint8_t>(local));
}

inline std::ostream&
operator<<(std::ostream& os, LocalAddress addr)
{
    os << static_cast<int>(addr);
    return os;
}

inline LocalAddress
toLocalAddress(gsl::byte byte)
{
    return static_cast<LocalAddress>(byte);
}

/**
 * Protocol structures describing packet headers. Note:
 * Each element _must_ be of a Type where sizeof(Type)==1.
 * or arrays of Type.
 * This will guarantee that no padding is inserted.
 */
namespace packet
{

template <typename HeaderType>
inline HeaderType*
toHeader(gsl::byte* b)
{
    return static_cast<HeaderType*>(static_cast<void*>(b));
}

struct GrantToken
{
    MessageType m_type;
    LocalAddress m_tokenReceiver;
};

struct ReturnToken
{
    MessageType m_type;
    LocalAddress m_src;
};

struct MasterStarted
{
    MessageType m_type;
};

struct MasterEnded
{
    MessageType m_type;
};

struct SendPacket
{
    MessageType m_type;
    LocalAddress m_destAddr;
    LocalAddress m_srcAddr;
};

struct MacUpdate
{
    MessageType m_type;
    LocalAddress m_destAddr;
    LocalAddress m_srcAddr;
    LocalAddress m_hintAddr;
    std::array<gsl::byte, 6> m_mac;
};
}

#endif /* SRC_INTERFACES_SERIALPROTOCOL_H_ */
