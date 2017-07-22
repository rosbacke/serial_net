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
 * VecQueue.h
 *
 *  Created on: 13 nov. 2016
 *      Author: mikaelr
 */

#ifndef SRC_UTILITY_VECQUEUE_H_
#define SRC_UTILITY_VECQUEUE_H_

#include <vector>

/**
 * Implement a queue that is expected to drain to empty from time to time.
 * This allows using an std::vector for storage. It does require the application
 * to ensure that the queue drains from time to time to avoid runaway
 * conditions.
 * Partial protection is built in to force elements to the beginning after a
 * while.
 */
template <class El>
class VecQueue
{
  public:
    VecQueue() : m_headPos(0){};
    ~VecQueue(){};

    template <int normLimit = 20>
    void push(const El& el)
    {
        if (m_store.size() > normLimit)
        {
            checkRenormalization();
        }
        m_store.push_back(el);
    }

    void pop()
    {
        ++m_headPos;
        check_empty();
    }
    void pop_back()
    {
        m_store.pop_back();
        check_empty();
    }

    El& front()
    {
        return m_store[m_headPos];
    }
    const El& front() const
    {
        return m_store[m_headPos];
    }

    std::size_t size() const
    {
        return m_store.size() - m_headPos;
    }
    bool empty() const
    {
        return m_store.empty();
    }

  private:
    // Invariants:
    // m_headPos <= m_store.size();
    // m_headPos point to the head element, or is == 0.
    // m_store.size() == 0 when the queue is empty.
    void check_empty()
    {
        if (m_headPos == m_store.size())
        {
            m_headPos = 0;
            m_store.clear();
        }
    }
    void checkRenormalization()
    {
        if (m_headPos > m_store.size() / 2)
        {
            // Force renormalization if the size is starting to grow.
            auto b = m_store.begin();
            m_store.erase(b, b + m_headPos);
            m_headPos = 0;
        }
    }

    std::vector<El> m_store;
    std::size_t m_headPos;
};

#endif /* SRC_UTILITY_VECQUEUE_H_ */
