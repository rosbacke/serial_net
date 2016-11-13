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
 * Event.cpp
 *
 *  Created on: 12 nov. 2016
 *      Author: mikaelr
 */

#include "Event.h"

Event::Event()
{
    // TODO Auto-generated constructor stub
}

Event::~Event()
{
    // TODO Auto-generated destructor stub
}

std::string
Event::toString(Id id)
{
#define CASE(x) \
    case Id::x: \
        return #x

    switch (id)
    {
        CASE(entry);
        CASE(exit);
        CASE(init);
        CASE(rx_grant_token);
        CASE(rx_client_packet);
        CASE(rx_return_token);
        CASE(timer_timeout);
    }
    return "";
}
