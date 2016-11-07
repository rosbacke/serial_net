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
 * PacketTypeCodec.h
 *
 *  Created on: 17 jul 2016
 *      Author: mikaelr
 */

#ifndef SRC_CORE_PACKETTYPECODEC_H_
#define SRC_CORE_PACKETTYPECODEC_H_

#include "interfaces/MsgEtherIf.h"
#include "interfaces/MsgHostIf.h"
#include "utility/Utility.h"

#include "interfaces/MasterTxIf.h"
#include "interfaces/SerialProtocol.h"

#include <array>
#include <cstdint>
#include <deque>
#include <functional>
#include <vector>

class MsgHostIf;
class WSDump;
class AddressCache;
class TxQueue;

class PacketTypeCodec : public MsgEtherIf::RxIf, public MasterRxIf
{
  public:
    struct RxPacket
    {
        ByteVec m_data;
        LocalAddress srcAddr;
        LocalAddress destAddr;
    };

    void setHostIf(MsgHostIf* msgHostIf)
    {
        m_msgHostIf = msgHostIf;
    }

    void setMasterEndedCB(std::function<void()> fkn)
    {
        m_masterEndedCB = fkn;
    }

    PacketTypeCodec(MsgEtherIf* msgEtherIf, TxQueue* tx,
                    LocalAddress ownAddress);

    virtual ~PacketTypeCodec();

    void sendPacket(const MsgHostIf::HostPkt& data, LocalAddress address);

    bool empty() const
    {
        return m_rxMsg.empty();
    }

    void deliverRxQueue();

    void setWsDump(WSDump* wsDump)
    {
        m_wsDump = wsDump;
    }

    void setAddressCache(AddressCache* ac)
    {
        m_cache = ac;
    }

  private:
    void rxRawPacket(const MsgEtherIf::EtherPkt& packet);

    void handleToken(const ByteVec& packet);

    // Implement MsgEtherIf::RxIf interface.
    virtual void msgEtherRx_newMsg(const MsgEtherIf::EtherPkt& packet) override;

    // Called by the master to set up reception of packets.
    virtual void regMasterRx(MasterRxIf::RxIf* rxIf) override
    {
        m_master = rxIf;
    }

    // Query low level sending software to know if a packet is being received.
    virtual bool packetRxInProgress() override
    {
        return m_msgEtherIf->packetRxInProgress();
    }

    MsgEtherIf* m_msgEtherIf;
    MsgHostIf* m_msgHostIf;
    TxQueue* m_txQueue;

    MasterRxIf::RxIf* m_master;
    WSDump* m_wsDump;

    LocalAddress m_ownAddress;
    std::deque<ByteVec> m_rxMsg;

    std::function<void()> m_masterEndedCB;
    AddressCache* m_cache = nullptr;
};

#endif /* SRC_CORE_PACKETTYPECODEC_H_ */
