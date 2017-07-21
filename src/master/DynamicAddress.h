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
 * DynamicAddress.h
 *
 *  Created on: 8 juli 2017
 *      Author: mikaelr
 */

#ifndef SRC_MASTER_DYNAMICADDRESS_H_
#define SRC_MASTER_DYNAMICADDRESS_H_

#include "interfaces/SerialProtocol.h"

class DynamicAddress
{
    using UniqueId = packet::UniqueId;

  public:
    DynamicAddress();

    DynamicAddress(UniqueId uniqueId, LocalAddress local)
        : m_uniqueId(uniqueId), m_local(local)
    {
    }

    ~DynamicAddress();

    LocalAddress local() const
    {
        return m_local;
    }

    UniqueId uniqueId() const
    {
        return m_uniqueId;
    }

  private:
    UniqueId m_uniqueId;
    LocalAddress m_local = LocalAddress::null_addr;
};

#endif /* SRC_MASTER_DYNAMICADDRESS_H_ */
