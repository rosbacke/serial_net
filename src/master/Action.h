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
 * Action.h
 *
 *  Created on: 13 nov. 2016
 *      Author: mikaelr
 */

#ifndef SRC_MASTER_ACTION_H_
#define SRC_MASTER_ACTION_H_

#include "interfaces/SerialProtocol.h"

#include <string>

class Action
{
  public:
    enum class Cmd
    {
        send_master_start,
        send_token,
        query_address,
        do_nothing,
    };

    enum class ReturnValue
    {
        ok,
        not_set,
        timeout, // General error reporting timeout.
        rx_token_no_packet,
        client_packet_started,
        token_timeout,
        address_query_done,
    };

    Action() : m_action(Cmd::do_nothing), m_address(LocalAddress::null_addr)
    {
    }

    Action(Cmd state, LocalAddress addr) : m_action(state), m_address(addr)
    {
    }

    bool doNothing() const
    {
        return m_action == Cmd::do_nothing;
    }

    static Action makeMasterStartAction()
    {
        return Action(Cmd::send_master_start, LocalAddress::null_addr);
    }

    static Action makeSendTokenAction(LocalAddress addr)
    {
        return Action(Cmd::send_token, addr);
    }

    static Action makeQueryAddressAction()
    {
        return Action(Cmd::query_address, LocalAddress::null_addr);
    }

    static Action makeDoNothingAction()
    {
        return Action(Cmd::do_nothing, LocalAddress::null_addr);
    }

    static std::string toString(Cmd cmd);

    static std::string toString(ReturnValue cmd);

    Cmd m_action;
    LocalAddress m_address;
    ReturnValue m_result = ReturnValue::not_set;
    std::function<void(ReturnValue)> m_reportCB = nullptr;
};

inline bool
operator>(const Action& lhs, const Action& rhs)
{
    return lhs.m_action > rhs.m_action;
}

#endif /* SRC_MASTER_ACTION_H_ */
