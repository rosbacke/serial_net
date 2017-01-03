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
 * PosixTunTapReal.h
 *
 *  Created on: 1 nov. 2016
 *      Author: mikaelr
 */

#ifndef SRC_HAL_POSIXTUNTAPREAL_H_
#define SRC_HAL_POSIXTUNTAPREAL_H_

#include "PosixIf.h"

class PosixTunTapReal : public PosixTunTapIf
{
  public:
    int ioctl_TUNSETIFF(int fd, void* ifr_p) final;

    /**
     * ioctl for accessing netdevice flags.
     */
    int ioctl_SIOCSIFFLAGS(int fd, void* ifr_p) final;

    /**
     * ioctl for accessing netdevice flags.
     */
    int ioctl_SIOCGIFFLAGS(int fd, void* ifr_p) final;
};

#endif /* SRC_HAL_POSIXTUNTAPREAL_H_ */
