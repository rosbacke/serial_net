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
 * TxQueue.h
 *
 *  Created on: 30 okt. 2016
 *      Author: mikaelr
 */

#ifndef SRC_CORE_TXQUEUE_H_
#define SRC_CORE_TXQUEUE_H_

#include <deque>

#include "../interfaces/MasterTxIf.h"
#include "utility/Utility.h"

#include "interfaces/MsgHostIf.h"

/**
 * Responsible for sending packets to the network.
 */
class TxQueue : public MsgHostIf::TxIf, public MasterTxIf
{
  public:
    TxQueue(MsgEtherIf* msgEtherIf, LocalAddress ownAddr)
        : m_msgEtherIf(msgEtherIf), m_ownAddress(ownAddr)
    {
    }
    ~TxQueue()
    {
    }

    void sendPacket(const MsgHostIf::HostPkt& data, LocalAddress address);

    // Implement MsgHostIf::TxIf interface.
    virtual void msgHostTx_sendPacket(const MsgHostIf::HostPkt& data,
                                      LocalAddress srcAddr,
                                      LocalAddress destAddr) override;

    virtual void
    msgHostTx_sendAddressUpdate(LocalAddress address,
                                std::array<gsl::byte, 6> mac) override;

    // Called by the master to send a packet. Contain everything except the
    // frame layer.
    virtual void sendMasterPacket(const ByteVec& packet) override;

    // Own client can send a packet.
    virtual bool sendClientPacket(bool useReturnToken) override;

  private:
    // Inform the master that we do not have a packet to send.
    void sendReturnToken();

    std::deque<ByteVec> m_txMsg;
    MsgEtherIf* m_msgEtherIf = nullptr;
    LocalAddress m_ownAddress;
};

#endif /* SRC_CORE_TXQUEUE_H_ */
