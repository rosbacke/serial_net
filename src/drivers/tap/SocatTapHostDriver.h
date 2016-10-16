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

#ifndef SRC_DRIVERS_TUN_SOCATTAPHOSTDRIVER_H_
#define SRC_DRIVERS_TUN_SOCATTAPHOSTDRIVER_H_

#include "interfaces/MsgHostIf.h"
#include "reactcpp.h"

#include <array>
#include <core/AddressCache.h>

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
class SocatTapHostDriver : public MsgHostIf
{
    using MACAddr = AddressCache::MacAddr;
    struct TapHeader;
    struct EtherHeader;
    struct Ipv4Header;
    struct EtherFooter;
    struct ArpIpv4Header;

  public:
    SocatTapHostDriver(int myAddr);
    virtual ~SocatTapHostDriver();

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

    void setAddressCache(AddressCache* ac)
    {
        m_cache = ac;
    }

  private:
    void setupCallback(React::Loop& mainLoop);
    void doRead(int fileDescriptor);
    void checkArp(const TapHeader* tap);

    int m_myAddr = -1;
    MsgHostIf::TxIf* m_txIf = nullptr;

    MACAddr m_myMAC;
    bool m_myMacIsSet = false;
    AddressCache* m_cache = nullptr;
};

#endif /* SRC_DRIVERS_TUN_SOCATTUNHOSTDRIVER_H_ */
