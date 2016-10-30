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

enum class MessageType
{
    grant_token,
    return_token,
    master_started,
    master_ended,
    send_packet,
    send_tun_packet,
    mac_update,
    lookup_address,
    address_data
};

/**
 * Address allocations:
 * 1 byte address between 0-255.
 * Address:
 * 0 : unallocated node, invalid address.
 * 1 : master node.
 * 2-254 allocated slots.
 * 255 broadcast.
 */

/**
 * Packet frame layout. Indicate offset into binary packet.
 */
// Grant token packet.
enum class FrameGrantToken
{
    msg_type,
    token_receiver, // Address of client who gets the token.
    max_num
};

enum class FrameReturnToken
{
    msg_type,
    max_num // Size of the packet.
};

//
enum class FrameSendPacket
{
    msg_type,
    dest_addr,  // Where the packet should go.
    token_hint, // Hint to master where the client suggest whom should have the
                // token next.
    packet_data // Where the actual data begins. Continues until the end of the
                // packet.
};

#endif /* SRC_INTERFACES_SERIALPROTOCOL_H_ */
