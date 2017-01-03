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
 * AddressLine.h
 *
 *  Created on: 20 okt. 2016
 *      Author: mikaelr
 */

#ifndef SRC_MASTER_ADDRESSLINE_H_
#define SRC_MASTER_ADDRESSLINE_H_

#include "interfaces/SerialProtocol.h"

/**
 * One row in the Addresses store. Represent a single local address.
 */
class AddressLine
{
  public:
    enum class State
    {
        active,    // Have recently transmitted packets. Might have more.
        idle,      // Have recently declined the token. Well behaved.
        badClient, // Have failed to return token and let the timeout pass.
        free       // This address does not respond to token request.
    };

    AddressLine(LocalAddress addr, State s = State::idle, bool dynamic = false)
        : m_state(s), m_address(addr), m_isDynamic(dynamic){};
    ~AddressLine(){};

    void setInit(State state)
    {
        m_state = state;
    }

    LocalAddress getAddr() const
    {
        return m_address;
    }

    State getState() const
    {
        return m_state;
    }

    bool removeDynamic() const
    {
        return m_isDynamic && m_state == State::free;
    }

    void setState(State st)
    {
        m_badCount = (st == State::badClient) ? m_badCount + 1 : 0;

        if (m_badCount > 3)
        {
            m_state = State::free;
        }
        else
        {
            m_state = st;
        }
    }

  private:
    State m_state = State::idle;
    int m_badCount = 0;
    LocalAddress m_address;
    bool m_isDynamic = false;
};

#endif /* SRC_MASTER_ADDRESSLINE_H_ */
