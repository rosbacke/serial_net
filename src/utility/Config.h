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
 * Config.h
 *
 *  Created on: 27 aug. 2016
 *      Author: mikaelr
 */

#ifndef SRC_UTILITY_CONFIG_H_
#define SRC_UTILITY_CONFIG_H_

#include <string>

/**
 * File for collecting  various configuration variables.
 * These are candidate for moving out to a separate
 * config system supported by configuration files.
 */
class Config
{
  public:
    Config();
    ~Config();

    /**
     * Read config.
     */
    void readConfig(const std::string& cfgFile);

    /**
     * Lowest address to check if somebody want to send.
     */
    int masterLowAddress() const
    {
        return 1;
    }

    /**
     * Highest address to check if somebody want to send.
     */
    int masterHighAddress() const
    {
        return 2;
    }

    /**
     * How long time should pass between each token query round.
     */
    double masterTokenCycleTime() const
    {
        return 0.05;
    }

    /**
     * How long time should pass from the master sending a grant token until
     * we receive it on our own serial port? (If this happens, the hardware is
     * defective)
     */
    double masterTokenGrantTx2RxTime() const
    {
        return 0.01;
    }

    /**
     * How long time does the client have to respond to a request before sending
     * a packet
     */
    double masterTokenClientTimeout() const
    {
        return 0.2;
    }

    /**
     * Maximum time (sec) a client is allowed to wait before sending a
     * packet or replying to the master. After this, the master assumes
     * control over the bus again.
     */
    // static constexpr double TOKEN_REPLY_TIMEOUT = 0.01;

    /**
     * Time between the round robin check of all the clients if they want to
     * send.
     */
    // static constexpr double PACKET_QUERY_INTERVAL = 0.1;

    static Config& instance();
};

#endif /* SRC_UTILITY_CONFIG_H_ */
