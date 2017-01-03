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
 * DynamicHandler.cpp
 *
 *  Created on: 25 nov. 2016
 *      Author: mikaelr
 */

#include "DynamicHandler.h"
#include "ActionHandler.h"
#include "MasterPacketTx.h"
#include "MasterScheduler.h"

#include "utility/Config.h"

DynamicHandler::DynamicHandler(EventLoop& loop, Config* cfg,
                               MasterScheduler* ms, MasterPacketTx* tx)
    : m_loop(loop), m_scheduler(ms), m_tx(tx)
{
    auto now = m_loop.now();
    m_freeAddresses.reserve(cfg->dynamicHighAddress() -
                            cfg->dynamicLowAddress() + 1);
    for (int i = cfg->dynamicHighAddress(); i >= cfg->dynamicLowAddress(); --i)
    {
        m_freeAddresses.push_back(toLocalAddress(i));
    }
    m_scheduler->addAction(Action::makeQueryAddressAction(), now);
}

DynamicHandler::~DynamicHandler()
{
}

void
DynamicHandler::receivedAddressRequest(const packet::AddressRequest& aReq)
{
    auto* addrLine = find(aReq.m_uniqueId);
    if (!addrLine && !m_freeAddresses.empty())
    {
        auto local = allocAddress(aReq.m_uniqueId);
        m_ah->addDynamic(local);
        addrLine = find(local);
    }
    if (addrLine)
    {
        m_tx->sendAddressReply(addrLine->m_local, aReq.m_uniqueId);
    }
}

LocalAddress
DynamicHandler::allocAddress(packet::UniqueId id)
{
    auto local = m_freeAddresses.back();
    m_allocAddresses.emplace_back(id, local);
    return local;
}

void
DynamicHandler::releaseAddress(LocalAddress local)
{
    auto i = std::find_if(
        m_allocAddresses.begin(), m_allocAddresses.end(),
        [&](const DynamicAddr& el) -> bool { return el.m_local == local; });
    if (i != m_allocAddresses.end())
    {
        m_freeAddresses.push_back(i->m_local);
        m_allocAddresses.erase(i);
    }
}

DynamicHandler::DynamicAddr*
DynamicHandler::find(LocalAddress local)
{
    auto i = std::find_if(
        m_allocAddresses.begin(), m_allocAddresses.end(),
        [&](const DynamicAddr& el) -> bool { return el.m_local == local; });
    return i == m_allocAddresses.end() ? nullptr : &(*i);
}

DynamicHandler::DynamicAddr*
DynamicHandler::find(UniqueId id)
{
    auto i = std::find_if(
        m_allocAddresses.begin(), m_allocAddresses.end(),
        [&](const DynamicAddr& el) -> bool { return el.m_uniqueId == id; });
    return i == m_allocAddresses.end() ? nullptr : &(*i);
}
