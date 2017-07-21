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
 * Event.h
 *
 *  Created on: 12 nov. 2016
 *      Author: mikaelr
 */

#ifndef SRC_MASTER_EVENT_H_
#define SRC_MASTER_EVENT_H_

#include <boost/variant.hpp>
#include <string>

#include <interfaces/SerialProtocol.h>

class Event
{
  public:
    enum class Id
    {
        init,
        rx_pkt_master_start_stop,
        rx_pkt_grant_token,
        rx_pkt_client_packet,
        rx_return_token,
        rx_pkt_address_request,
        timer_timeout,
        check_new_command,
    };

    Event(Id id) : m_id(id)
    {
    }
    Event();
    ~Event();

    static std::string toString(Id id);

    Id m_id;

    boost::variant<packet::AddressRequest> eventData;
};

#endif /* SRC_MASTER_EVENT_H_ */
