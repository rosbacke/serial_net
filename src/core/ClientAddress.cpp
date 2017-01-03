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
 * ClientAddress.cpp
 *
 *  Created on: 13 nov. 2016
 *      Author: mikaelr
 */

#include "ClientAddress.h"
#include "TxQueue.h"
#include "utility/Log.h"
#include <boost/format.hpp>
#include <sstream>

#include <random>

ClientAddress::ClientAddress(TxQueue* txQueue) : m_txQueue(txQueue)
{
    setupUniqueId();
}

ClientAddress::~ClientAddress()
{
}

void
ClientAddress::rxDiscoveryPacket(const packet::AddressDiscovery& packet)
{
    if (m_txQueue->msgHostTx_clientAddress() != LocalAddress::null_addr)
    {
        return;
    }
    // TODO: Random delay.
    packet::AddressRequest p;
    p.m_type = MessageType::address_request;
    std::copy(m_uniqueId.begin(), m_uniqueId.end(), p.m_uniqueId.begin());
    // Skip CRC for now.
    m_txQueue->sendMasterPacket(packet::fromHeader(p));
}

void
ClientAddress::rxAddrReplyPacket(const packet::AddressReply& packet)
{
    if (packet.m_uniqueId == m_uniqueId)
    {
        m_txQueue->setAddress(packet.m_assigned);
    }
    // TODO: Set up timeouts, invalidation etc.
}

void
ClientAddress::setupUniqueId()
{
    std::random_device rd;  // obtain a random number from hardware
    std::mt19937 eng(rd()); // seed the generator
    std::uniform_int_distribution<> distr(0, 255); // define the range

    for (auto& i : m_uniqueId)
    {
        i = gsl::to_byte(static_cast<uint8_t>(distr(eng)));
    }

    const auto& u = m_uniqueId;
    auto rn = [&](auto index) -> int { return static_cast<int>(u[index]); };
    std::stringstream ss;
    ss << "Random unique id: "
       << boost::format("%02x %02x %02x %02x %02x %02x %02x %02x") % rn(0) %
              rn(1) % rn(2) % rn(3) % rn(4) % rn(5) % rn(6) % rn(7);

    LOG_INFO << ss.str();
}

void
ClientAddress::rxToken(const packet::GrantToken& packet)
{
    // TODO: Update timing for own address.
}
