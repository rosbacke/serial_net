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
 * SNConfig.cpp
 *
 *  Created on: 5 aug. 2016
 *      Author: mikaelr
 */

#include "SNConfig.h"

#include "utility/Log.h"

std::string
SNConfig::toString(Mode mode)
{
#define CASE(x)   \
    case Mode::x: \
        return #x
    switch (mode)
    {
        CASE(unknown);
        CASE(none);
        CASE(std_io);
        CASE(raw_pty);
        CASE(socat_tun);
        CASE(socat_tap);
        CASE(tun);
        CASE(tap);
        CASE(setup_tap);
        CASE(remove_tap);
        CASE(mode_max_num);
    }
    return "";
#undef CASE
}

#define IF_MODE(x)      \
    if (mode == #x)     \
    {                   \
        return Mode::x; \
    }                   \
    else

SNConfig::Mode
SNConfig::toMode(std::string mode)
{
    IF_MODE(unknown)
    IF_MODE(none)
    IF_MODE(std_io)
    IF_MODE(raw_pty)
    IF_MODE(socat_tun)  //
    IF_MODE(socat_tap)  //
    IF_MODE(tun)        //
    IF_MODE(tap)        //
    IF_MODE(setup_tap)  //
    IF_MODE(remove_tap) //
    {
        return Mode::unknown;
    }
}
#undef IF_MODE

namespace
{
using RtsOptions = SerialByteEther::RtsOptions;
}

#define IF_MODE(x)            \
    if (str == #x)            \
    {                         \
        return RtsOptions::x; \
    }                         \
    else

RtsOptions
SNConfig::toOption(const std::string& str)
{
    IF_MODE(None)
    IF_MODE(pulldown)
    IF_MODE(rs485_te)
    {
        return RtsOptions::None;
    }
}
#undef IF_MODE

std::string
SNConfig::toString(RtsOptions option)
{
#define CASE(x)         \
    case RtsOptions::x: \
        return #x

    switch (option)
    {
        CASE(None);
        CASE(pulldown);
        CASE(rs485_te);
    }
    return "";
}

void
SNConfig::setupSNConfig(boost::program_options::variables_map& vm)
{
    m_etherPath = vm.count("serial-device") > 0
                      ? vm["serial-device"].as<std::string>()
                      : "";
    m_mode = toMode(vm["mode"].as<std::string>());

    m_on_if_up = vm["on-if-up"].as<std::string>();
    m_on_if_down = vm["on-if-down"].as<std::string>();

    m_startMaster = vm.count("master") > 0;

    m_rtsOption = SNConfig::toOption(vm["serial-options"].as<std::string>());
    m_user = vm["user"].as<std::string>();
    m_group = vm["group"].as<std::string>();
    m_tapName = vm["tap-name"].as<std::string>();

    if (vm.count("wsdump") > 0)
    {
        m_wsDumpPath = vm["wsdump"].as<std::string>();
    }

    int myAddr = vm["address"].as<int>();

    if (myAddr < 0 || myAddr > 32)
    {
        LOG_ERROR << "Illegal local address " << myAddr;
        throw std::runtime_error("Illegal static address.");
    }
    if (myAddr == 0 && m_startMaster)
    {
        myAddr = 1;
    }
    m_staticAddr = static_cast<LocalAddress>(myAddr);

    m_peerAddr = vm.count("peer_address") > 0
                     ? toLocalAddress(vm["peer_address"].as<int>())
                     : LocalAddress::broadcast;

    LOG_INFO << "mode: " << SNConfig::toString(m_mode);
    LOG_INFO << "addr: " << myAddr;
    LOG_INFO << "peer_addr: " << m_peerAddr;

    m_masterTimeout = vm["mtimeout"].as<int>();
}
