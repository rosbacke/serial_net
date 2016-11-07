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
 * Addresses.cpp
 *
 *  Created on: 20 okt. 2016
 *      Author: mikaelr
 */

#include "Addresses.h"

Addresses::Addresses(int minAddr, int maxAddr, React::Loop& loop, Config* cfg)
    : m_minAddr(minAddr), m_maxAddr(maxAddr), m_loop(loop), m_config(cfg)
{
    using State = AddressLine::State;

    m_table.resize(m_maxAddr - m_minAddr + 1);
    double now = loop.now();
    for (int i = m_minAddr; i <= m_maxAddr; ++i)
    {
        m_table[i].setInit(State::active);
        m_ready.push(QueueEl(static_cast<LocalAddress>(i), now));
    }
}

Addresses::~Addresses()
{
}

double
Addresses::nextTime(AddressLine::State state)
{
    using State = AddressLine::State;
    auto now = m_loop.now();
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

AddressLine*
Addresses::find(LocalAddress address)
{
    auto offset = static_cast<int>(address) - m_minAddr;
    if (offset < static_cast<int>(m_table.size()) && offset >= 0)
    {
        return &m_table[offset];
    }
    else
    {
        return nullptr;
    }
}

void
Addresses::gotReturnToken()
{
    auto addr = m_ready.top().m_address;

    auto line = find(addr);
    line->setState(AddressLine::State::idle);

    if (m_frontInProgress)
    {
        m_ready.pop();
        m_frontInProgress = false;
        m_ready.push(QueueEl(addr, nextTime(line->getState())));
    }
}

void
Addresses::tokenTimeout()
{
    auto addr = m_ready.top().m_address;
    auto line = find(addr);
    line->setState(AddressLine::State::badClient);

    if (m_frontInProgress)
    {
        m_ready.pop();
        m_frontInProgress = false;
        m_ready.push(QueueEl(addr, nextTime(line->getState())));
    }
}

void
Addresses::packetStarted()
{
    auto addr = m_ready.top().m_address;
    auto line = find(addr);
    line->setState(AddressLine::State::active);

    if (m_frontInProgress)
    {
        m_ready.pop();
        m_frontInProgress = false;
        m_ready.push(QueueEl(addr, nextTime(line->getState())));
    }
}

Addresses::Action
Addresses::nextAction()
{
    auto now = m_loop.now();
    const QueueEl& a = m_ready.top();
    if (a.m_nextTime <= now)
    {
        m_frontInProgress = true;
        return Action::makeSendTokenAction(a.m_address);
    }
    else
    {
        return Action::makeDelayAction(a.m_nextTime);
    }
}

std::string
toString(Addresses::Action::State state)
{
    using State = Addresses::Action::State;
    switch (state)
    {
    case State::delay:
        return "delay";
    case State::send_token:
        return "send_token";
    }
    return "error";
}
