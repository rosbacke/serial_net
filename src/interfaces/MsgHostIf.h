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
 * MsgHostIf.h
 *
 *  Created on: 14 aug. 2016
 *      Author: mikaelr
 */

#ifndef SRC_CORE_MSGHOSTIF_H_
#define SRC_CORE_MSGHOSTIF_H_

#include "SerialProtocol.h"
#include "utility/Utility.h"

#include <array>
#include <cstdint>
#include <vector>

/**
 * Interface for the driver facing the host side.
 */
class MsgHostIf
{
  public:
    // Represent the raw bytes to be passed on the host side.
    class HostPkt : public gsl::span<const gsl::byte>
    {
      public:
        HostPkt(const gsl::byte* start, std::size_t length)
            : gsl::span<const gsl::byte>(start, length)
        {
        }
    };

    // The core implements this function.
    virtual void msgHostTx_sendPacket(const HostPkt& data,
                                      LocalAddress destAddr,
                                      ChannelType chType) = 0;

    virtual void msgHostTx_sendMacUpdate(std::array<gsl::byte, 6> mac) = 0;

    // Return the currently assigned local address.
    virtual LocalAddress msgHostTx_clientAddress() const = 0;

    class RxIf
    {
      public:
        /**
         * Called when a packet was received from the serial net.
         */
        virtual void packetReceivedFromNet(const ByteVec& data,
                                           LocalAddress srcAddr,
                                           LocalAddress destAddr,
                                           ChannelType chType) = 0;
    };

    /**
     * Inform the driver where it is supposed to send its packets.
     * @param rxIf  Interface where received packets are posted.
     * @param ChannelType type of packets to forward.
     */
    virtual void setRxHandler(RxIf* rxIf, ChannelType chType) = 0;

    /**
      * Interface for receiving notifications when the local address changes.
      */
    class AddrChange
    {
      public:
        virtual void msgHostRx_newAddr(LocalAddress addr) = 0;
    };

    virtual void setAddrUpdateHandler(AddrChange* addrUpdateHandler) = 0;

    virtual ~MsgHostIf(){};
};

#endif /* SRC_CORE_MSGHOSTIF_H_ */
