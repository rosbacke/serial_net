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

#include "MsgEtherIf.h"

/**
 * Defined message type for the system.
 */
enum class MessageType : uint8_t
{
    grant_token, // 0
    return_token,
    master_started,
    master_ended,
    send_packet,
    mac_update,        // 5
    address_discovery, // 6
    address_request,
    address_reply
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

inline std::ostream&
operator<<(std::ostream& os, MessageType msgType)
{
    return os << static_cast<int>(msgType);
}

static inline std::string
toString(MessageType mt)
{
#define CASE(x)          \
    case MessageType::x: \
        return #x
    switch (mt)
    {
        CASE(grant_token);
        CASE(return_token);
        CASE(master_started);
        CASE(master_ended);
        CASE(send_packet);
        CASE(mac_update);
        CASE(address_discovery);
        CASE(address_request);
        CASE(address_reply);
    }
    return "";
#undef CASE
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

inline LocalAddress
toLocalAddress(int byte)
{
    return static_cast<LocalAddress>(byte);
}

enum class ChannelType : uint8_t
{
    illegal_type,
    raw_stream,
    tun_format,
    tap_format
};

static inline std::string
toString(ChannelType mt)
{
#define CASE(x)          \
    case ChannelType::x: \
        return #x
    switch (mt)
    {
        CASE(illegal_type);
        CASE(raw_stream);
        CASE(tun_format);
        CASE(tap_format);
    }
    return "";
#undef CASE
}

/**
 * Protocol structures describing packet headers. Note:
 * Each element _must_ be of a Type where sizeof(Type)==1.
 * or arrays of Type.
 * This will guarantee that no padding is inserted.
 */
namespace packet
{
using UniqueId = std::array<gsl::byte, 8>;
using Crc32 = std::array<gsl::byte, 4>;

template <typename HeaderType>
inline HeaderType*
toHeader(gsl::byte* b)
{
    return static_cast<HeaderType*>(static_cast<void*>(b));
}

template <typename HeaderType>
inline const HeaderType*
toHeader(const gsl::byte* b)
{
    return static_cast<const HeaderType*>(static_cast<const void*>(b));
}

template <typename HeaderType>
inline const HeaderType*
toHeader(const MsgEtherIf::EtherPkt& p)
{
    assert(p.size() >= (int)sizeof(HeaderType));
    return toHeader<HeaderType>(p.data());
}

template <typename HeaderType>
inline MsgEtherIf::EtherPkt
fromHeader(const HeaderType& header)
{
    MsgEtherIf::EtherPkt ep(
        static_cast<const gsl::byte*>(static_cast<const void*>(&header)),
        sizeof(HeaderType));
    return ep;
}

struct GrantToken
{
    GrantToken(LocalAddress tr)
        : m_type(MessageType::grant_token), m_tokenReceiver(tr)
    {
    }
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
    ChannelType m_chType;
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

struct AddressDiscovery
{
    MessageType m_type;
    uint8_t m_randomDelay; // Hold off answer randomly up to this amount of ms.
    uint8_t m_randomHoldOff; // 0xff -> p = 1.0, 0 -> p = 0.0
};

struct AddressRequest
{
    MessageType m_type;
    UniqueId m_uniqueId;
    ChannelType m_channelType;
    Crc32 m_crc32;
};

struct AddressReply
{
    MessageType m_type;
    LocalAddress m_assigned;
    UniqueId m_uniqueId;
};
}

#endif /* SRC_INTERFACES_SERIALPROTOCOL_H_ */
