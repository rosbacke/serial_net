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
 * ClientAddress.h
 *
 *  Created on: 13 nov. 2016
 *      Author: mikaelr
 */

#ifndef SRC_CORE_CLIENTADDRESS_H_
#define SRC_CORE_CLIENTADDRESS_H_

#include "interfaces/SerialProtocol.h"

class TxQueue;

/**
 * Handle the client local address. Supply the local address and
 * have the logic to negotiate a dynamic address.
 */
class ClientAddress
{
  public:
    ClientAddress(TxQueue* txQueue);
    ~ClientAddress();

    LocalAddress getAddr() const;

    void rxToken(const packet::GrantToken& packet);

    void rxDiscoveryPacket(const packet::AddressDiscovery& packet);

    void rxAddrReplyPacket(const packet::AddressReply& packet);

    void setupUniqueId();

  private:
    enum class AddrState
    {
        requireDynamic,
        gotDynamic,
        gotStatic
    };

    LocalAddress m_addr;
    AddrState m_addrState;
    packet::UniqueId m_uniqueId;
    TxQueue* m_txQueue;
};

#endif /* SRC_CORE_CLIENTADDRESS_H_ */
