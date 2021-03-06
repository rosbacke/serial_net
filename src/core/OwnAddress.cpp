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
 * OwnAddress.cpp
 *
 *  Created on: 24 jan. 2017
 *      Author: mikaelr
 */

#include "OwnAddress.h"
#include "utility/Log.h"

OwnAddress::OwnAddress()
{
    // TODO Auto-generated constructor stub
}

OwnAddress::~OwnAddress()
{
    // TODO Auto-generated destructor stub
}

void
OwnAddress::set(LocalAddress addr)
{
    const bool change = (m_addr != addr);
    m_addr = addr;
    if (m_addrChangeIf && change)
    {
        m_addrChangeIf->msgHostRx_newAddr(m_addr);
    }
}

void
OwnAddress::masterStarted()
{
    if (m_dynamic)
    {
        LOG_INFO << "Detected master start. Set own address to 0";
        set(LocalAddress::null_addr);
    }
}
