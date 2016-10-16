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
 * Log.cpp
 *
 *  Created on: 29 jul 2016
 *      Author: mikaelr
 */

#include "Log.h"

#include <time.h>

#include <sys/types.h>
#include <unistd.h>

Log::Log() : m_level(Level::debug), m_pid(::getpid())
{
    struct timespec ts;
    ::clock_gettime(CLOCK_REALTIME, &ts);

    m_timeBase = uint64_t(ts.tv_sec) * 1000000 + ts.tv_nsec / 1000;
}

std::string
Log::header() const
{
    struct timespec ts;
    ::clock_gettime(CLOCK_REALTIME, &ts);

    uint64_t now = ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
    uint64_t delta = now - m_timeBase;
    int sec = delta / 1000000;
    int usec = delta % 1000000;

    std::stringstream ss;
    char buf[256];
    sprintf(buf, "%05d %09d.%06d", m_pid, sec, usec);
    return std::string(buf);
}

void
Log::writeLog(const std::string& line, Level level)
{
    if (static_cast<int>(level) >= static_cast<int>(m_level))
    {
        std::cerr << header() << " " << line << '\n';
        std::cerr.flush();
    }
}
