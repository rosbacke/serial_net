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
 * PosixFd.cpp
 *
 *  Created on: 4 nov. 2016
 *      Author: mikaelr
 */

#include "PosixFd.h"

#include "PosixIf.h"

PosixFd::~PosixFd()
{
    set(-1);
}

void
PosixFd::set(int fd)
{
    if (m_fd >= 0 && m_posixIf)
    {
        m_posixIf->close(m_fd);
    }
    m_fd = fd;
}
