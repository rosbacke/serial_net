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

#include "AddressCache.h"
#include "core/PacketTypeCodec.h"

#include <utility/Utility.h>

namespace
{
constexpr auto t = gsl::to_byte<0xff>();
constexpr AddressCache::MacAddr broadcastMac{t, t, t, t, t, t};
}

AddressCache::AddressCache()
{
}

int
AddressCache::getLocalAddress(const MacAddr& addr)
{
    if (addr == broadcastMac)
    {
        return 0xff;
    }
    if (m_myLocalAddr != -1 && m_myMac == addr)
    {
        return m_myLocalAddr;
    }
    auto iter =
        std::find_if(m_table.begin(), m_table.end(),
                     [&](const Entry& el) { return addr == el.second; });
    if (iter == m_table.end())
    {
        return -1;
    }
    return iter->first;
}

void
AddressCache::setAddress(int local, const MacAddr& mac)
{
    if (local == 0xff)
    {
        return;
    }
    auto iter =
        std::find_if(m_table.begin(), m_table.end(),
                     [&](const Entry& el) { return local == el.first; });
    if (iter != m_table.end())
    {
        *iter = std::make_pair(local, mac);
    }
    else
    {
        m_table.push_back(std::make_pair(local, mac));
    }
}

void
AddressCache::updateMyMAC(int local, const MacAddr& mac)
{
    if (m_myLocalAddr != -1)
    {
        return;
    }
    m_myMac = mac;
    m_myLocalAddr = local;
}

std::pair<bool, AddressCache::MacAddr>
AddressCache::getMac(int local)
{
    if (local == 0xff)
    {
        return std::make_pair(true, broadcastMac);
    }
    auto iter =
        std::find_if(m_table.begin(), m_table.end(),
                     [&](const Entry& el) { return local == el.first; });
    if (iter != m_table.end())
    {
        return std::make_pair(true, iter->second);
    }
    else
    {
        return std::make_pair(false, broadcastMac);
    }
}
