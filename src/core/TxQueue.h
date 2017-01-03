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

#include "interfaces/MasterTxIf.h"
#include "utility/Log.h"
#include "utility/Utility.h"

#include "interfaces/MsgHostIf.h"

/**
 * Responsible for sending packets to the network.
 */
class TxQueue : public MsgHostIf, public MasterTxIf
{
  public:
    TxQueue(MsgEtherIf* msgEtherIf, LocalAddress ownAddr)
        : m_msgEtherIf(msgEtherIf), m_ownAddress(ownAddr),
          m_gotDynamicAddress(ownAddr == LocalAddress::null_addr)
    {
        LOG_INFO << "Set own address to :" << ownAddr << " at start.";
    }
    ~TxQueue()
    {
    }

    void setAddress(LocalAddress addr)
    {
        LOG_INFO << "Set own address to :" << addr;
        m_ownAddress = addr;
        if (m_addrChangeIf)
        {
            m_addrChangeIf->msgHostRx_newAddr(m_ownAddress);
        }
    }

    void sendPacket(const MsgHostIf::HostPkt& data, LocalAddress address);

    // Implement MsgHostIf::TxIf interface.
    virtual void msgHostTx_sendPacket(const MsgHostIf::HostPkt& data,
                                      LocalAddress destAddr) override;

    virtual void msgHostTx_sendMacUpdate(std::array<gsl::byte, 6> mac) override;

    virtual LocalAddress msgHostTx_clientAddress() const override
    {
        return m_ownAddress;
    }

    // Called by the master to send a packet. Contain everything except the
    // frame layer.
    virtual void sendMasterPacket(const MsgEtherIf::EtherPkt& packet) override;

    // Own client can send a packet.
    virtual void sendClientPacket() override;

    virtual LocalAddress clientAddress() const override
    {
        return m_ownAddress;
    }

    // Return true if the Tx queue is empty.
    virtual bool txQueueEmpty() const override
    {
        return m_txMsg.empty();
    }

    // Send either a client packet or a return token.
    void sendClientPacketOrReturnToken();

    virtual void setRxHandler(RxIf* rxIf)
    {
        m_rxIf = rxIf;
    }

    MsgHostIf::RxIf* getRxIf() const
    {
        return m_rxIf;
    }

    virtual void setAddrUpdateHandler(MsgHostIf::AddrChange* ac)
    {
        m_addrChangeIf = ac;
    }

    // Indicate that the master on this bus has started again.
    // Reset any dynamically assigned addresses.
    void masterStarted();

  private:
    // Inform the master that we do not have a packet to send.
    void sendReturnToken();

    std::deque<ByteVec> m_txMsg;
    MsgEtherIf* m_msgEtherIf = nullptr;
    LocalAddress m_ownAddress = LocalAddress::null_addr;
    bool m_gotDynamicAddress;

    MsgHostIf::RxIf* m_rxIf = nullptr;
    MsgHostIf::AddrChange* m_addrChangeIf = nullptr;
};

#endif /* SRC_CORE_TXQUEUE_H_ */
