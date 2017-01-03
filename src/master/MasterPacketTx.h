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
 * MasterPacketTx.h
 *
 *  Created on: 25 nov. 2016
 *      Author: mikaelr
 */

#ifndef SRC_MASTER_MASTERPACKETTX_H_
#define SRC_MASTER_MASTERPACKETTX_H_

#include "interfaces/MasterTxIf.h"

class MasterPacketTx
{
  public:
    MasterPacketTx(MasterTxIf* masterTx) : m_masterTx(masterTx)
    {
    }

    ~MasterPacketTx()
    {
    }

    // Send the token to the next client.
    void sendToken(LocalAddress destAddr);

    void sendMasterStop();
    void sendMasterStart();
    void sendAddressDiscovery();

    void sendAddressReply(LocalAddress local, packet::UniqueId id);

    // Return true if there are no packets queued for transmission.
    bool txQueueEmpty() const
    {
        return m_masterTx->txQueueEmpty();
    }

    // When the own client can send a packet.
    void sendClientPacket()
    {
        m_masterTx->sendClientPacket();
    }

    LocalAddress clientAddress() const
    {
        return m_masterTx->clientAddress();
    }

  private:
    MasterTxIf* m_masterTx;
};

#endif /* SRC_MASTER_MASTERPACKETTX_H_ */
