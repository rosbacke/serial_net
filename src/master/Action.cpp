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
 * Action.cpp
 *
 *  Created on: 13 nov. 2016
 *      Author: mikaelr
 */

#include "Action.h"

std::string
Action::toString(Cmd cmd)
{
    using Cmd = Action::Cmd;
    switch (cmd)
    {
    case Cmd::send_master_start:
        return "send_master_start";
    case Cmd::send_token:
        return "send_token";
    case Cmd::query_address:
        return "query_address";
    case Cmd::do_nothing:
        return "do_nothing";
    }
    return "error";
}

#define CASE(x) \
    case RV::x: \
        return #x

std::string
Action::toString(ReturnValue rv)
{
    using RV = Action::ReturnValue;
    switch (rv)
    {
        CASE(ok);
        CASE(not_set);
        CASE(timeout);
        CASE(rx_token_no_packet);
        CASE(client_packet_started);
        CASE(token_timeout);
        CASE(address_query_done);
    };
    return "error";
}
#undef CASE
