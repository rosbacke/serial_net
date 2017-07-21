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
#include "interfaces/TimeServiceIf.h"
#include "utility/Log.h"

ActionHandler::ActionHandler(TimeServiceIf& ts, Config* cfg,
                             std::function<void(const Action&)> triggerAction)
    : m_ts(ts), m_config(cfg), m_triggerNewAction(triggerAction)
{
}

void
ActionHandler::reportActionResult(Action::ReturnValue rv)
{
    // LOG_DEBUG << "Action result: " << Action::toString(rv);

    auto addr = m_currentAction.m_address;
    m_currentAction = Action::makeDoNothingAction();

    using RV = Action::ReturnValue;
    switch (rv)
    {
    case RV::client_packet_started:
        m_dynamic->updateAddressLine(addr, AddressLine::State::active);
        break;

    case RV::rx_token_no_packet:
        m_dynamic->updateAddressLine(addr, AddressLine::State::idle);
        break;

    case RV::token_timeout:
        m_dynamic->updateAddressLine(addr, AddressLine::State::badClient);
        break;

    case RV::address_query_done:
        m_dynamic->addressDiscoveryIsDone();
        break;

    case RV::ok:
        break;

    default:
        assert(0);
    }
    checkNewAction();
}

void
ActionHandler::checkNewAction()
{
    if (m_currentAction.doNothing() && m_triggerNewAction)
    {
        auto now = m_ts.now();
        if (m_scheduler.workToDo(now))
        {
            m_currentAction = m_scheduler.front();
            m_scheduler.pop();
            m_triggerNewAction(m_currentAction);
        }
        else
        {
            auto nextTime = m_scheduler.readyTime();
            if (nextTime < 1.0)
                return;
            else
            {
                m_delayAction = m_ts.makeTimeoutAbs(
                    nextTime, [&]() { this->checkNewAction(); });
            }
        }
    }
}
