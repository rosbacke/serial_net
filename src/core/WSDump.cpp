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
 * WSDump.cpp
 *
 *  Created on: 5 okt. 2016
 *      Author: mikaelr
 */

#include "WSDump.h"
#include "utility/Log.h"
#include "utility/Utility.h"

#include <boost/format.hpp>

#include <fstream>
#include <iostream>
#include <string>

WSDump::WSDump(std::string fname) : m_os(nullptr)
{
    if (fname != "")
    {
        m_fstream.open(fname, std::ios::trunc | std::ios::out);
        if (m_fstream.is_open())
        {
            m_os = &m_fstream;
        }
    }
}

WSDump::~WSDump()
{
    if (m_os)
    {
        m_fstream.close();
    }
}

// Write out text readable by text2pcap : text2pcap -t '%s.' -l 147 - -
void
WSDump::rxPacket(const std::vector<gsl::byte>& data)
{
    const bool doZeroBasedTimes = true;

    const auto cols = 0x10_sz;
    if (!m_os)
    {
        return;
    }
    auto size = data.size();
    auto index = 0_sz;
    while (index < size)
    {
        auto now = Utility::now();
        if (doZeroBasedTimes)
        {
            static double base = now;
            now -= base;
        }
        *m_os << Utility::timeStr(now);
        *m_os << boost::format(" %06x") % index;
        for (auto col = 0_sz; col < cols && index < size; ++col, ++index)
        {
            *m_os << boost::format(" %02x") % int(data[index]);
        }
        *m_os << '\n';
    }
    m_os->flush();
}
