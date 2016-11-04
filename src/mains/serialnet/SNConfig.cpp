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

SNConfig::SNConfig()
{
}

SNConfig::~SNConfig()
{
}

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
        CASE(std_in);
        CASE(std_out);
        CASE(std_io);
        CASE(socat_tun);
        CASE(socat_tap);
        CASE(tap);
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
    IF_MODE(std_in)
    IF_MODE(std_out)
    IF_MODE(std_io)
    IF_MODE(socat_tun) //
    IF_MODE(socat_tap) //
    IF_MODE(tap)       //
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
