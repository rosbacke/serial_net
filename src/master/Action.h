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

class Action
{
  public:
    enum class Cmd
    {
        send_token,
        delay,
        query_address,
    };

    Action(Cmd state, LocalAddress addr, double time)
        : m_action(state), m_address(addr), m_nextTime(time)
    {
    }

    static Action makeDelayAction(double time)
    {
        return Action(Cmd::delay, LocalAddress::null_addr, time);
    }

    static Action makeSendTokenAction(LocalAddress addr)
    {
        return Action(Cmd::send_token, addr, 0.0);
    }

    static Action makeQueryAddressAction()
    {
        return Action(Cmd::query_address, LocalAddress::null_addr, 0.0);
    }

    Cmd m_action;
    LocalAddress m_address;
    double m_nextTime;
};

inline bool
operator>(const Action& lhs, const Action& rhs)
{
    return lhs.m_action > rhs.m_action;
}

#endif /* SRC_MASTER_ACTION_H_ */
