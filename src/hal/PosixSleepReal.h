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
 * PosixSleepIfReal.h
 *
 *  Created on: 29 okt. 2016
 *      Author: mikaelr
 */

#ifndef SRC_HAL_POSIXSLEEPREAL_H_
#define SRC_HAL_POSIXSLEEPREAL_H_

#include "PosixIf.h"

#include <unistd.h>

class PosixSleepReal final : public PosixSleepIf
{
  public:
    int usleep(useconds_t usec) override
    {
        return ::usleep(usec);
    }
};

#endif /* SRC_HAL_POSIXSLEEPREAL_H_ */
