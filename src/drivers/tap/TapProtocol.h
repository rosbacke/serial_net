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
 * TapProtocol.h
 *
 *  Created on: 27 okt. 2016
 *      Author: mikaelr
 */

#ifndef SRC_DRIVERS_TAP_TAPPROTOCOL_H_
#define SRC_DRIVERS_TAP_TAPPROTOCOL_H_

#include "core/AddressCache.h"
#include "interfaces/MsgHostIf.h"

class PosixFileIf;

class TapProtocol
{
    using MACAddr = AddressCache::MacAddr;
    struct TapHeader;
    struct EtherHeader;
    struct Ipv4Header;
    struct EtherFooter;
    struct ArpIpv4Header;

  public:
    TapProtocol(int myAddr, AddressCache* ac, PosixFileIf* pfi)
        : m_myAddr(myAddr), m_cache(ac), m_posixFileIf(pfi)
    {
    }

    ~TapProtocol()
    {
    }

    void setTx(MsgHostIf::TxIf* txIf)
    {
        m_txIf = txIf;
    }

    void doRead(int fd);
    /**
     * Called when a packet was received from the serial net.
     */
    void packetReceived(int fd, const ByteVec& data, int srcAddr, int destAddr);

  private:
    void checkArp(const TapHeader* tap);

    int m_myAddr = -1;
    MsgHostIf::TxIf* m_txIf = nullptr;
    AddressCache* m_cache = nullptr;
    PosixFileIf* m_posixFileIf;
};

#endif /* SRC_DRIVERS_TAP_TAPPROTOCOL_H_ */
