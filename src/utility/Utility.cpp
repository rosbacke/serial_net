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
 * Utility.cpp
 *
 *  Created on: 6 okt. 2016
 *      Author: mikaelr
 */

#include "Utility.h"

#include <boost/format.hpp>

#include <sstream>

#include <cmath>
#include <ctime>

Utility::Utility()
{
}

Utility::~Utility()
{
}

double
Utility::now()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    double time = static_cast<double>(ts.tv_sec);
    time += static_cast<double>(ts.tv_nsec) * 1e-9;
    return time;
}

std::string
Utility::timeStr(double time)
{
    const uint64_t oneE9 = 1000000000;
    // We know we have a time with about 10 digits seconds,
    // and that it is not greater than 2^32-1.
    // Hence multiplying with 1e9 will fit inside an uint64_t.
    uint64_t val = static_cast<uint64_t>(std::floor(time * 1e9 + 0.5));
    uint32_t sec = val / oneE9;
    uint32_t nsec = val % oneE9;
    std::stringstream ss;
    ss << boost::format("%d.%09d") % sec % nsec;
    return ss.str();
}
