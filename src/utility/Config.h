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
        return 16;
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
        return 0.1;
    }

    /**
     * How long time does the client have to respond to a request before sending
     * a packet
     */
    double masterTokenClientTimeout() const
    {
        return 0.05;
    }

    /**
     * How long delay until next token passing, for a single address
     * after it has sent a packet
     */
    double addrDelayTokenPacketSent() const
    {
        return 0.0001; // Really short. Might have more packets.
    }

    /**
     * How long delay until next token passing, for a single address
     * after it has missed to reply to a token.
     */
    double addrDelayTokenTimeout() const
    {
        return 10.0; // Really long. This node is a nuisance to the network.
    }

    /**
     * We have given up on this address. Check it occasionally in case a
     * new unit is connected.
     *
     */
    double addrDelayFreeAddress() const
    {
        return 30.0; // Really long. We do not expect any node here.
    }

    /**
     * How long delay until next token passing, for a single address
     * after it has returned the token.
     */
    double addrDelayTokenReturn() const
    {
        return 0.1; // A bit longer. Focus on the ones currently sending.
    }

    /**
     * What is the period time between address query attempts?
     */
    double addrQueryPeriod() const
    {
        return 5.0; // A bit longer. Shouldn't be new units during normal
                    // operration.
    }

    static Config& instance();
};

#endif /* SRC_UTILITY_CONFIG_H_ */
