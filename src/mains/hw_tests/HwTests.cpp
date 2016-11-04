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
 * HwTests.cpp
 *
 *  Created on: 2 nov. 2016
 *      Author: mikaelr
 */

#include "HwTests.h"

#include "drivers/serial/PosixSerialUtils.h"
#include "hal/PosixFileReal.h"
#include "hal/PosixSerialReal.h"
#include "hal/PosixSleepReal.h"
#include "utility/Log.h"

#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

void
HwTests::doSetRTS(std::string device, int value, int timeout)
{
    PosixFileReal pfr;
    PosixSerialReal psr;
    PosixSleepReal pslr;

    auto val = value ? PosixSerialUtils::IOState::asserted
                     : PosixSerialUtils::IOState::negated;
    int fd = pfr.open(device.c_str(), O_RDWR);
    if (fd < 0)
    {
        LOG_ERROR << "Failed to open device " << device
                  << " error:" << std::strerror(errno);
        return;
    }
    PosixSerialUtils::setRTS(psr, fd, val);
    if (timeout == -1)
    {
        while (1)
            ;
    }
    else
    {
        pslr.sleep(timeout);
    }
    pfr.close(fd);
}
