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
 * Log.h
 *
 *  Created on: 29 jul 2016
 *      Author: mikaelr
 */

#ifndef SRC_CORE_LOG_H_
#define SRC_CORE_LOG_H_

#include <iostream>
#include <sstream>

class Log
{
  public:
    enum class Level
    {
        trace,
        debug,
        info,
        warning,
        error
    };

    static Log& instance()
    {
        static Log log;
        return log;
    }

    Log();
    ~Log(){};

    void writeLog(const std::string& line, Level level);

    std::string header() const;
    uint64_t m_timeBase; // us
    Level m_level;
    int m_pid;
};

namespace
{
class LogLine
{
  public:
    LogLine(std::ostream& os, Log::Level level) : m_ss(), m_level(level)
    {
    }

    template <class T>
    std::ostream& operator<<(const T& t)
    {
        m_ss << t;
        return m_ss;
    }
    ~LogLine()
    {
        Log::instance().writeLog(m_ss.str(), m_level);
    }

  private:
    std::stringstream m_ss;
    Log::Level m_level;
};
}

#define LOG(x) LogLine(std::cerr, x)

#define LOG_TRACE LOG(Log::Level::trace)
#define LOG_DEBUG LOG(Log::Level::debug)
#define LOG_INFO LOG(Log::Level::info)
#define LOG_WARN LOG(Log::Level::warning)
#define LOG_ERROR LOG(Log::Level::error)

#endif /* SRC_CORE_LOG_H_ */
