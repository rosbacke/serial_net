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

//
// Created by mikaelr on 2016-10-14.
//

#ifndef SERIAL_NET_ADDRESSCACHE_H
#define SERIAL_NET_ADDRESSCACHE_H

#include <gsl/gsl_byte>

#include "interfaces/SerialProtocol.h"
#include <array>
#include <memory>
#include <utility>
#include <vector>

class PacketTypeCodec;

/**
 * A cache that remember the mapping between Ethernet and local addresses.
 */
class AddressCache
{
  public:
    using MacAddr = std::array<gsl::byte, 6>;
    AddressCache(){};

    /**
     * Return the local address that has the given MAC address.
     * @param addr MAC address.
     * @return Local address, -1 if not found.
     */
    LocalAddress getLocalAddress(const MacAddr& addr);

    void setAddress(LocalAddress local, const MacAddr& mac);

    void updateMyMAC(LocalAddress local, const MacAddr& mac);

    std::pair<bool, MacAddr> getMac(LocalAddress local);

  private:
    using Entry = std::pair<LocalAddress, MacAddr>;
    std::vector<Entry> m_table;
    MacAddr m_myMac;
    LocalAddress m_myLocalAddr = LocalAddress::null_addr;
    PacketTypeCodec* m_ptCodec = nullptr;
};

#endif // SERIAL_NET_ADDRESSCACHE_H
