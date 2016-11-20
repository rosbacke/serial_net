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
 * MsgEtherIf.h
 *
 *  Created on: 29 jul 2016
 *      Author: mikaelr
 */

#ifndef SRC_CORE_MSGETHERIF_H_
#define SRC_CORE_MSGETHERIF_H_

#include "utility/Utility.h"
#include <cstdint>
#include <vector>

#include "gsl/gsl"

/**
 * Interface for sending and receiving packets from the low level byte
 * interface.
 *
 */
class MsgEtherIf
{
  public:
    class EtherPkt : public gsl::span<const gsl::byte>
    {
      public:
        EtherPkt(const gsl::byte* start, std::size_t length)
            : gsl::span<const gsl::byte>(start, length)
        {
        }
        explicit EtherPkt(const ByteVec& bv)
            : gsl::span<const gsl::byte>(bv.data(), bv.size())
        {
        }
    };

    /**
     * Interface for receiving a new packet from the medium.
     */
    class RxIf
    {
      public:
        virtual void msgEtherRx_newMsg(const EtherPkt& msg) = 0;
    };

    MsgEtherIf();
    virtual ~MsgEtherIf();

    // Send a message to the ether.
    virtual void sendMsg(const EtherPkt& msg) = 0;

    // Register a receiver.
    virtual void addClient(RxIf* cb) = 0;

    // Allow an implementer to give a hint that a packet is being received.
    virtual bool packetRxInProgress()
    {
        return false;
    }
};

#endif /* SRC_CORE_MSGETHERIF_H_ */
