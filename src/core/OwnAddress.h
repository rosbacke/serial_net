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
 * OwnAddress.h
 *
 *  Created on: 24 jan. 2017
 *      Author: mikaelr
 */

#ifndef SRC_CORE_OWNADDRESS_H_
#define SRC_CORE_OWNADDRESS_H_

#include "interfaces/MsgHostIf.h"
#include "interfaces/SerialProtocol.h"

class OwnAddress
{
  public:
    OwnAddress();
    explicit OwnAddress(LocalAddress la)
        : m_addr(la), m_dynamic(la == LocalAddress::null_addr){};

    ~OwnAddress();

    LocalAddress addr() const
    {
        return m_addr;
    }

    bool valid() const
    {
        return m_addr != LocalAddress::null_addr;
    }

    void set(LocalAddress addr);

    void setListener(MsgHostIf::AddrChange* ac)
    {
        m_addrChangeIf = ac;
    }

    void masterStarted();

  private:
    LocalAddress m_addr = LocalAddress::null_addr;
    bool m_dynamic = false;
    MsgHostIf::AddrChange* m_addrChangeIf = nullptr;
};

#endif /* SRC_CORE_OWNADDRESS_H_ */
