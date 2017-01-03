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

ActionHandler::ActionHandler(EventLoop& loop, Config* cfg, MasterScheduler& ms,
                             DynamicHandler* dh)
    : m_minAddr(cfg->staticLowAddress()), m_maxAddr(cfg->staticHighAddress()),
      m_loop(loop), m_config(cfg), m_scheduler(ms), m_dynamic(dh)
{
    double now = loop.now();
    for (int i = m_minAddr; i <= m_maxAddr; ++i)
    {
        m_table.emplace_back(toLocalAddress(i));
        m_scheduler.addAction(
            Action::makeSendTokenAction(static_cast<LocalAddress>(i)), now);
    }
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
    auto i = std::find_if(
        m_table.begin(), m_table.end(),
        [&](const auto& el) -> bool { return address == el.getAddr(); });
    if (i != m_table.end())
    {
        return &(*i);
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

    AddressLine* line = find(addr);
    line->setState(newState);
    if (m_frontInProgress)
    {
        m_scheduler.pop();
        m_frontInProgress = false;
    }
    if (line->removeDynamic())
    {
        removeLine(line);
        m_dynamic->releaseAddress(addr);
    }
    else
    {
        m_scheduler.addAction(Action::makeSendTokenAction(addr),
                              nextTime(line->getState()));
    }
}

void
ActionHandler::removeLine(AddressLine* line)
{
    size_t offset = line - m_table.data();
    m_table.erase(m_table.begin() + offset);
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

void
ActionHandler::addDynamic(LocalAddress local)
{
    m_table.emplace_back(local, AddressLine::State::idle, true);
    m_scheduler.addAction(Action::makeSendTokenAction(local),
                          m_loop.now() + 0.01);
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
