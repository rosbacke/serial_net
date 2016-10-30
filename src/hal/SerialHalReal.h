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
 * SerialHalReal2.h
 *
 *  Created on: 29 okt. 2016
 *      Author: mikaelr
 */

#ifndef SRC_HAL_SERIALHALREAL_H_
#define SRC_HAL_SERIALHALREAL_H_

#include "PosixFileReal.h"
#include "PosixSerialReal.h"
#include "PosixSleepReal.h"
#include "drivers/serial/SerialByteEther.h"

class SerialHalReal
{
  public:
    PosixFileReal m_file;
    PosixSleepReal m_sleep;
    PosixSerialReal m_serial;

    SerialByteEther::SerialHal get()
    {
        SerialByteEther::SerialHal ser(&m_file, &m_sleep, &m_serial);
        return ser;
    }
};

#endif /* SRC_HAL_SERIALHALREAL_H_ */
