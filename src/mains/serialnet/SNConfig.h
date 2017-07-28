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

#include <boost/program_options.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/options_description.hpp>

#include "drivers/serial/SerialByteEther.h"

#include "interfaces/SerialProtocol.h"

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
        std_io,
        raw_pty,
        socat_tun,
        socat_tap,
        tun,
        tap,
        setup_tap,
        remove_tap,
        mode_max_num
    };
    static std::string toString(Mode mode);
    static Mode toMode(std::string mode);

    using RtsOptions = SerialByteEther::RtsOptions;

    static std::string toString(RtsOptions option);
    static RtsOptions toOption(const std::string& str);

    static double constexpr packetReplyTimeout = 0.01;

    SNConfig()
    {
    }
    ~SNConfig()
    {
    }

    void setupSNConfig(boost::program_options::variables_map& vm);

    std::string m_etherPath;
    Mode m_mode = Mode::unknown;
    std::string m_on_if_up;
    std::string m_on_if_down;
    bool m_startMaster = false;
    SNConfig::RtsOptions m_rtsOption = SNConfig::RtsOptions::None;
    std::string m_wsDumpPath;
    std::string m_tapName;

    // Local address derived from the command line.
    LocalAddress m_staticAddr = LocalAddress::null_addr;

    // Destination address for stdin mode.
    LocalAddress m_peerAddr = LocalAddress::null_addr;

    int m_masterTimeout = 0;

    std::string m_user;
    std::string m_group;

    // Path for status information. (understand '~/' as home folder)
    std::string m_rootPath = "~/.serialnet";
};

#endif /* SRC_MAINS_SERIALNET_SNCONFIG_H_ */
