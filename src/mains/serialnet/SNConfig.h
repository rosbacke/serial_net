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
 * SNConfig.h
 *
 *  Created on: 5 aug. 2016
 *      Author: mikaelr
 */

#ifndef SRC_MAINS_SERIALNET_SNCONFIG_H_
#define SRC_MAINS_SERIALNET_SNCONFIG_H_

#include <string>

#include "drivers/serial/SerialByteEther.h"

/**
 * Captured configuration from the command line arguments
 */
class SNConfig
{
  public:
    enum class Mode
    {
        unknown,
        none,
        std_in,
        std_out,
        std_io,
        socat_tun,
        socat_tap,
        tap,
        mode_max_num
    };
    static std::string toString(Mode mode);
    static Mode toMode(std::string mode);

    using RtsOptions = SerialByteEther::RtsOptions;

    static std::string toString(RtsOptions option);
    static RtsOptions toOption(const std::string& str);

    static double constexpr packetReplyTimeout = 0.01;

    SNConfig();
    ~SNConfig();
};

#endif /* SRC_MAINS_SERIALNET_SNCONFIG_H_ */
