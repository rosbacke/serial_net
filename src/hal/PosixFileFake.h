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
 * PosixFileFake.h
 *
 *  Created on: 4 nov. 2016
 *      Author: mikaelr
 */

#ifndef SRC_HAL_POSIXFILEFAKE_H_
#define SRC_HAL_POSIXFILEFAKE_H_

#include "PosixIf.h"
#include <string>
#include <unistd.h>

class PosixFileFake : public PosixFileIf
{
  public:
    int open(const char* pathname, int flags) final
    {
        m_path = pathname;
        m_flags = flags;
        m_cnt++;
        return m_res;
    }
    int open(const char* pathname, int flags, mode_t mode) final
    {
        m_path = pathname;
        m_flags = flags;
        m_mode = mode;
        m_cnt++;
        return m_res;
    }
    std::ptrdiff_t read(int fd, void* buf, size_t size) const final
    {
        m_fd = fd;
        m_buf = buf;
        m_maxLen = size;
        m_cnt++;
        return m_res;
    }
    std::ptrdiff_t write(int fd, const void* buf, size_t size) final
    {
        m_fd = fd;
        m_buf = buf;
        m_maxLen = size;
        m_cnt++;
        return m_res;
    }

    int close(int fd) final
    {
        m_fd = fd;
        m_cnt++;
        return m_res;
    }
    int system(const char*) final
    {
        return -1;
    }

    std::string m_path;
    ssize_t m_res;
    int m_flags;
    uint32_t m_mode;

    mutable int m_fd;
    mutable const void* m_buf;
    mutable std::size_t m_maxLen;
    mutable int m_cnt;
};

#endif /* SRC_HAL_POSIXFILEFAKE_H_ */
