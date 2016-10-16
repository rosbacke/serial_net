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

#include "ByteBuf.h"

#include "SerialProtocol.h"
#include "interfaces/MsgEtherIf.h"
#include "interfaces/MsgHostIf.h"

#include "utility/Utility.h"

#include <array>
#include <cstdint>
#include <deque>
#include <functional>
#include <vector>

class MsgHostIf;
class WSDump;
class AddressCache;

/**
 * Interface class implemented by the class supplying packets to
 * the master.
 */
class MasterChannelIf
{
  public:
    class RxIf
    {
      public:
        virtual void masterPacketReceived(MessageType type,
                                          const ByteVec& packet) = 0;
    };

    // Called by the master to send a packet. Contain everything except the
    // frame layer.
    virtual void sendMasterPacket(const ByteVec& packet) = 0;

    // When the own client can send a packet. return true if packet was sent.
    virtual bool sendClientPacket() = 0;

    // Return true if the client can deduce that a packet is currently
    // being received.
    virtual bool packetRxInProgress() = 0;

    // Called by the master to set up reception of packets.
    virtual void regMasterRx(RxIf* rxIf) = 0;

    virtual ~MasterChannelIf(){};
};

class PacketTypeCodec : public MsgEtherIf::RxIf,
                        public MsgHostIf::TxIf,
                        public MasterChannelIf
{
  public:
    struct RxPacket
    {
        ByteVec m_data;
        int srcAddr;
        int destAddr;
    };

    void setHostIf(MsgHostIf* msgHostIf)
    {
        m_msgHostIf = msgHostIf;
    }

    void setMasterEndedCB(std::function<void()> fkn)
    {
        m_masterEndedCB = fkn;
    }

    PacketTypeCodec(MsgEtherIf* msgEtherIf, int ownAddress);

    virtual ~PacketTypeCodec();

    void rxRawPacket(const ByteBuf& bb);

    void sendPacket(const ByteVec& data, int address);

    bool empty() const
    {
        return m_rxMsg.empty();
    }

    void deliverRxQueue();

    void setWsDump(WSDump* wsDump)
    {
        m_wsDump = wsDump;
    }

    virtual bool packetRxInProgress() override
    {
        return m_msgEtherIf->packetRxInProgress();
    }

    void setAddressCache(AddressCache* ac)
    {
        m_cache = ac;
    }

  private:
    void sendReturnToken();

    void handleToken(const ByteVec& packet);

    // Implement MsgEtherIf::RxIf interface.
    virtual void msgEtherRx_newMsg(const ByteVec& msg) override;

    // Implement MsgHostIf::TxIf interface.
    virtual void msgHostTx_sendPacket(const ByteVec& data, int srcAddr,
                                      int destAddr) override;

    virtual void
    msgHostTx_sendAddressUpdate(int address,
                                std::array<gsl::byte, 6> mac) override;

    // Called by the master to send a packet. Contain everything except the
    // frame layer.
    virtual void sendMasterPacket(const ByteVec& packet) override;

    // Own client can send a packet.
    virtual bool sendClientPacket() override;

    // Called by the master to set up reception of packets.
    virtual void regMasterRx(MasterChannelIf::RxIf* rxIf) override
    {
        m_master = rxIf;
    }

    MsgEtherIf* m_msgEtherIf;
    MsgHostIf* m_msgHostIf;

    MasterChannelIf::RxIf* m_master;
    WSDump* m_wsDump;

    int m_ownAddress;
    std::deque<ByteBuf> m_rxMsg;
    std::deque<ByteBuf> m_txMsg;

    std::function<void()> m_masterEndedCB;
    AddressCache* m_cache = nullptr;
};

#endif /* SRC_CORE_PACKETTYPECODEC_H_ */
