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
 * ByteBuf.h
 *
 *  Created on: 14 jul 2016
 *      Author: mikaelr
 */

#ifndef SRC_CORE_BYTEBUF_H_
#define SRC_CORE_BYTEBUF_H_

#include <algorithm>
#include <gsl/gsl>
#include <stdint.h>
#include <string>
#include <vector>

#include "utility/Utility.h"

/**
 * Encapsulate an array of bytes.
 */
class ByteBuf
{
  public:
    ByteBuf();
    explicit ByteBuf(const std::vector<gsl::byte>& buf) : m_buf(buf)
    {
    }
    explicit ByteBuf(const std::string& str) : m_buf(str.size())
    {
        m_buf.resize(0);
        for (auto el : str)
        {
            m_buf.push_back(static_cast<gsl::byte>(el));
        }
    }
    ByteBuf(const ByteBuf& buf) : m_buf(buf.m_buf)
    {
    }

    ~ByteBuf();

    const std::vector<gsl::byte>& get() const
    {
        return m_buf;
    }

    const gsl::byte* data() const
    {
        return m_buf.data();
    }
    std::string str() const
    {
        std::string t;
        for (auto el : m_buf)
        {
            t.push_back(static_cast<char>(el));
        }
        return t;
    }

    bool empty() const
    {
        return m_buf.empty();
    }

    std::size_t size() const
    {
        return m_buf.size();
    }

    gsl::byte operator[](std::size_t index) const
    {
        return m_buf[index];
    }

  private:
    std::vector<gsl::byte> m_buf;
};

#endif /* SRC_CORE_BYTEBUF_H_ */
