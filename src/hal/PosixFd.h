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
 * PosixFd.h
 *
 *  Created on: 4 nov. 2016
 *      Author: mikaelr
 */

#ifndef SRC_HAL_POSIXFD_H_
#define SRC_HAL_POSIXFD_H_

class PosixFileIf;

/**
 * Encapsulate a Posix file descriptor with ownership of the fd.
 * Make sure the fd is closed when destructed.
 */
class PosixFd
{
  public:
    /// Default construct a file descriptor with fd == -1.
    PosixFd(PosixFileIf* posixIf) : m_fd(-1), m_posixIf(posixIf)
    {
    }

    static PosixFd makeFd(int fd, PosixFileIf* pf)
    {
        return PosixFd(fd, pf);
    }

    // Set a new fd, making sure to close the old one.
    void set(int fd);

    int get() const
    {
        return m_fd;
    }

    // Let this class emulate normal behavour of a file descriptor
    // and act as a normal integer.
    operator int() const
    {
        return m_fd;
    }

    ~PosixFd();

  private:
    PosixFd(int fd, PosixFileIf* posixIf) : m_fd(fd), m_posixIf(posixIf)
    {
    }

    int m_fd;
    PosixFileIf* m_posixIf;
};

#endif /* SRC_HAL_POSIXFD_H_ */
