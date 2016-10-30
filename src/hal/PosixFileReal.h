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
 * PosixFileIfReal.h
 *
 *  Created on: 29 okt. 2016
 *      Author: mikaelr
 */

#ifndef SRC_HAL_POSIXFILEREAL_H_
#define SRC_HAL_POSIXFILEREAL_H_

#include "PosixIf.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

class PosixFileReal final : public PosixFileIf
{
  public:
    int open(const char* pathname, int flags) override
    {
        return ::open(pathname, flags);
    }
    int open(const char* pathname, int flags, mode_t mode) override
    {
        return ::open(pathname, flags, mode);
    }
    ssize_t read(int fd, void* buf, size_t size) override
    {
        return ::read(fd, buf, size);
    }
    ssize_t write(int fd, const void* buf, size_t size) override
    {
        return ::write(fd, buf, size);
    }
    int close(int fd) override
    {
        return ::close(fd);
    }
};

#endif /* SRC_HAL_POSIXFILEREAL_H_ */
