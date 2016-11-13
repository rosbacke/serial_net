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
 * MasterChannelIf.h
 *
 *  Created on: 30 okt. 2016
 *      Author: mikaelr
 */

#ifndef SRC_INTERFACES_MASTERTXIF_H_
#define SRC_INTERFACES_MASTERTXIF_H_

#include "MsgEtherIf.h"
#include "SerialProtocol.h"
#include "utility/Utility.h"

/**
 * Interface class implemented by the class sending packets from the master
 * to the network.
 */
class MasterTxIf
{
  public:
    // Called by the master to send a packet. Contain everything except the
    // frame layer.
    virtual void sendMasterPacket(const ByteVec& packet) = 0;

    // Return true if there are no packets queued for transmission.
    virtual bool txQueueEmpty() const = 0;

    // When the own client can send a packet. return true if packet was sent.
    virtual void sendClientPacket() = 0;

    virtual ~MasterTxIf(){};
};

/**
 * Interface class implemented by the class supplying packets to
 * the master.
 */
class MasterRxIf
{
  public:
    class RxIf
    {
      public:
        virtual void
        masterPacketReceived(MessageType type,
                             const MsgEtherIf::EtherPkt& packet) = 0;
    };

    // Called by the master to set up reception of packets.
    virtual void regMasterRx(RxIf* rxIf) = 0;

    // Return true if the client can deduce that a packet is currently
    // being received.
    virtual bool packetRxInProgress() = 0;

    virtual ~MasterRxIf(){};
};

#endif /* SRC_INTERFACES_MASTERTXIF_H_ */
