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

#include "ActionHandler.h"

#include "MasterScheduler.h"

ActionHandler::ActionHandler(React::Loop& loop, Config* cfg)
    : m_minAddr(cfg->masterLowAddress()), m_maxAddr(cfg->masterHighAddress()),
      m_loop(loop), m_config(cfg)
{
    using State = AddressLine::State;

    m_table.resize(m_maxAddr - m_minAddr + 1);
    double now = loop.now();
    for (int i = m_minAddr; i <= m_maxAddr; ++i)
    {
        m_table[i].setInit(State::active);
        m_scheduler.addAction(
            Action::makeSendTokenAction(static_cast<LocalAddress>(i)), now);
    }
    m_scheduler.addAction(Action::makeQueryAddressAction(), now);
}

ActionHandler::~ActionHandler()
{
}

double
ActionHandler::nextTime(AddressLine::State state)
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
ActionHandler::find(LocalAddress address)
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
ActionHandler::updateAddressLine(AddressLine::State newState)
{
    auto addr = m_scheduler.active().m_action.m_address;

    auto line = find(addr);
    line->setState(newState);

    if (m_frontInProgress)
    {
        m_scheduler.pop();
        m_frontInProgress = false;
        m_scheduler.addAction(Action::makeSendTokenAction(addr),
                              nextTime(line->getState()));
    }
}

void
ActionHandler::gotReturnToken()
{
    updateAddressLine(AddressLine::State::idle);
}

void
ActionHandler::tokenTimeout()
{
    updateAddressLine(AddressLine::State::badClient);
}

void
ActionHandler::packetStarted()
{
    updateAddressLine(AddressLine::State::active);
}

void
ActionHandler::addressQueryDone()
{
    if (m_frontInProgress)
    {
        m_scheduler.pop();
        m_frontInProgress = false;
        auto nextTime = m_loop.now() + m_config->addrQueryPeriod();
        m_scheduler.addAction(Action::makeQueryAddressAction(), nextTime);
    }
}

Action
ActionHandler::nextAction()
{
    auto now = m_loop.now();
    auto& a = m_scheduler.active();
    if (a.m_nextTime <= now)
    {
        m_frontInProgress = true;
        return a.m_action;
    }
    else
    {
        return Action::makeDelayAction(a.m_nextTime);
    }
}

std::string
toString(Action::Cmd state)
{
    using State = Action::Cmd;
    switch (state)
    {
    case State::delay:
        return "delay";
    case State::send_token:
        return "send_token";
    case State::query_address:
        return "query_address";
    }
    return "error";
}
