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

DynamicHandler::DynamicHandler(TimeServiceIf& ts, Config* cfg, MasterPacketTx* tx)
    : m_minAddr(cfg->staticLowAddress()), m_maxAddr(cfg->staticHighAddress()),
      m_ts(ts), m_tx(tx), m_config(cfg)
{
}

void
DynamicHandler::start()
{

    for (int i = m_minAddr; i <= m_maxAddr; ++i)
    {
        m_table.emplace_back(toLocalAddress(i));
        m_ah->postActionNow(
            Action::makeSendTokenAction(static_cast<LocalAddress>(i)));
    }
    m_freeAddresses.reserve(m_config->dynamicHighAddress() -
                            m_config->dynamicLowAddress() + 1);
    for (int i = m_config->dynamicHighAddress();
         i >= m_config->dynamicLowAddress(); --i)
    {
        m_freeAddresses.push_back(toLocalAddress(i));
    }
    m_ah->postActionNow(Action::makeQueryAddressAction());
}

DynamicHandler::~DynamicHandler()
{
}

void
DynamicHandler::removeLine(AddressLine* line)
{
    size_t offset = line - m_table.data();
    m_table.erase(m_table.begin() + offset);
}

void
DynamicHandler::receivedAddressRequest(const packet::AddressRequest& aReq)
{
    auto* addrLine = findLine(aReq.m_uniqueId);
    if (!addrLine && !m_freeAddresses.empty())
    {
        auto localAddr = allocAddress(aReq.m_uniqueId);
        m_table.emplace_back(localAddr, AddressLine::State::idle, true,
                             aReq.m_uniqueId);
        m_ah->postActionNow(Action::makeSendTokenAction(localAddr));
        addrLine = findLine(localAddr);
    }
    if (addrLine)
    {
        m_tx->sendAddressReply(addrLine->getAddr(), aReq.m_uniqueId);
    }
}

LocalAddress
DynamicHandler::allocAddress(packet::UniqueId id)
{
    if (m_freeAddresses.empty())
        return LocalAddress::null_addr;

    auto local = m_freeAddresses.back();
    m_freeAddresses.pop_back();
    return local;
}

AddressLine*
DynamicHandler::findLine(UniqueId id)
{
    auto i = std::find_if(
        m_table.begin(), m_table.end(),
        [&](const AddressLine& el) -> bool { return el.uniqueId() == id; });
    return i == m_table.end() ? nullptr : &(*i);
}

AddressLine*
DynamicHandler::findLine(LocalAddress address)
{
    auto i = std::find_if(
        m_table.begin(), m_table.end(),
        [&](const auto& el) -> bool { return address == el.getAddr(); });
    return i != m_table.end() ? &(*i) : nullptr;
}

void
DynamicHandler::addressDiscoveryIsDone()
{
    auto nextTime = m_ts.now() + m_config->addrQueryPeriod();
    m_timer = m_ts.makeTimeoutAbs(nextTime, [&]() {
        m_ah->postActionNow(Action::makeQueryAddressAction());

    });
}

void
DynamicHandler::updateAddressLine(LocalAddress addr,
                                  AddressLine::State newState)
{
    AddressLine* line = findLine(addr);
    line->setState(newState);
    if (line->removeDynamic())
    {
        m_freeAddresses.push_back(line->getAddr());
        removeLine(line);
    }
    else
    {
        m_ah->postAction(Action::makeSendTokenAction(addr),
                         nextTime(line->getState()));
    }
}

double
DynamicHandler::nextTime(AddressLine::State state)
{
    using State = AddressLine::State;
    auto now = m_ts.now();
    switch (state)
    {
    case State::active:
        return now + m_config->addrDelayTokenPacketSent();
    case State::idle:
        return now + m_config->addrDelayTokenReturn();
    case State::badClient:
        return now + m_config->addrDelayTokenTimeout();
    case State::free:
        return now + m_config->addrDelayFreeAddress();
    }
    return 0.0;
}
