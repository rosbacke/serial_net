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
 * MasterUtils.cpp
 *
 *  Created on: 12 nov. 2016
 *      Author: mikaelr
 */

#include "MasterUtils.h"
// #include "Timer.h"
#include "utility/Log.h"
#include <memory>

MasterUtils::MasterUtils()
{
    // TODO Auto-generated constructor stub
}

MasterUtils::~MasterUtils()
{
    // TODO Auto-generated destructor stub
}

Timer::Timer(EventLoop& loop) : m_loop(loop)
{
    m_timeoutHelper = std::make_shared<void*>(nullptr);
}

void
Timer::makeTimeout(double timeout, std::function<void()> fkn)
{
    auto weakPtr = std::weak_ptr<void*>(m_timeoutHelper);
    m_tokenTimeout = m_loop.onTimeout(timeout, [this, weakPtr, fkn]() -> bool {
        auto ptr = weakPtr.lock();
        if (ptr)
        {
            m_tokenTimeout.reset();
            fkn();
        }
        else
        {
            LOG_WARN << "Failed timeout cb.";
        }
        return false;
    });
    LOG_TRACE << "Timeout setup.";
}

void
Timer::makeTimeoutAbs(double timeout, std::function<void()> fkn)
{
    auto weakPtr = std::weak_ptr<void*>(m_timeoutHelper);
    m_tokenTimeout = m_loop.onTimeout(
        timeout - m_loop.now(), [this, weakPtr, fkn]() -> bool {
            auto ptr = weakPtr.lock();
            if (ptr)
            {
                m_tokenTimeout.reset();
                fkn();
            }
            else
            {
                LOG_WARN << "Failed timeoutabs cb.";
            }
            return false;
        });
    LOG_TRACE << "TimeoutAbs setup.";
}

void
Timer::cleanTimeout()
{
    LOG_TRACE << "Timeout cleanup.";

    if (m_tokenTimeout)
    {
        m_tokenTimeout->cancel();
        m_tokenTimeout.reset();
    }
}
