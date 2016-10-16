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
 * TunHostDriver.h
 *
 *  Created on: 21 sep. 2016
 *      Author: mikaelr
 */

#ifndef SRC_DRIVERS_TUN_SOCATTUNHOSTDRIVER_H_
#define SRC_DRIVERS_TUN_SOCATTUNHOSTDRIVER_H_

#include "interfaces/MsgHostIf.h"
#include "reactcpp.h"

#include <array>

/**
 * Simple host side driver using stdin/stdout. It is intended to be connected
 * as a TUN device via the socat utility.
 * It has some severe restrictions when it comes to TCP/IP due to the missing
 * ethernet address in the TUN interface. These are:
 * - All but the masters must be end nodes. They can not route a packet further.
 * - All end nodes must have the master as the next hop for the default route.
 * - The serial_net must be one network with a common network address.
 * - The host part of the IP address must correspond to the local address.
 *
 * The reason for this is that serial_net uses the IP address to infer the local
 * address to send to/receive from.
 *
 */
class SocatTunHostDriver : public MsgHostIf
{
  public:
    SocatTunHostDriver(int myAddr);
    virtual ~SocatTunHostDriver();

    void startTransfer(MsgHostIf::TxIf* txIf, React::Loop& loop);

    /**
     * Called when a packet was received from the serial net.
     */
    virtual void packetReceived(const ByteVec& data, int srcAddr,
                                int destAddr) override;

    /**
     * Inform the driver where it is supposed to send its packets.
     */
    virtual void setTxHandler(TxIf* txIf) override
    {
        m_txIf = txIf;
    }

  private:
    enum class ReadType
    {
        header,
        data
    };
    // Read up to and including the dest addr of the IPv4.
    static const constexpr int headerLen = 4 + 20;
    std::array<uint8_t, headerLen> m_readHeader;
    ByteVec m_rxTunPacket;

    ReadType m_readType = ReadType::header;

    void setupCallback(React::Loop& mainLoop);
    void doRead(int fileDescriptor);

    int m_myAddr;
    MsgHostIf::TxIf* m_txIf;
};

#endif /* SRC_DRIVERS_TUN_SOCATTUNHOSTDRIVER_H_ */
