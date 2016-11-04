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
 * PosixSerialUtils.cpp
 *
 *  Created on: 2 nov. 2016
 *      Author: mikaelr
 */

#include "PosixSerialUtils.h"
#include "hal/PosixIf.h"
#include "utility/Log.h"

#include <cstring>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <linux/serial.h>

int
PosixSerialUtils::setRTS(PosixSerialIf& serial, int fd, IOState state)
{
    int status;
    int res;
    res = serial.ioctl_TIOCMGET(fd, &status);
    if (res == -1)
    {
        LOG_ERROR << "setRTS(): TIOCMGET, failed : " << std::strerror(errno);
        return res;
    }
    switch (state)
    {
    case IOState::asserted:
        status |= TIOCM_RTS;
        break;
    case IOState::negated:
        status &= ~TIOCM_RTS;
        break;
    }

    serial.ioctl_TIOCMSET(fd, &status);
    if (res == -1)
    {
        LOG_ERROR << "setRTS(): TIOCMSET, failed : " << std::strerror(errno);
        return res;
    }
    return res;
}

int
PosixSerialUtils::setRS485Mode(PosixSerialIf& serial, int fd, bool enable,
                               bool rxEnabledDuringTx, IOState duringTx,
                               IOState afterTx)
{
    struct serial_rs485 rs485;
    ::memset(&rs485, 0, sizeof(rs485));
    if (enable)
    {
        rs485.flags |= SER_RS485_ENABLED;
        if (rxEnabledDuringTx)
        {
            rs485.flags |= SER_RS485_RX_DURING_TX;
        }
        if (duringTx == IOState::asserted)
        {
            rs485.flags |= SER_RS485_RTS_ON_SEND;
        }
        if (afterTx == IOState::asserted)
        {
            rs485.flags |= SER_RS485_RTS_AFTER_SEND;
        }
    }
    return serial.ioctl_TIOCSRS485(fd, &rs485);
}

int
PosixSerialUtils::set8N1Termios(PosixSerialIf& serial, int fd, Baudrate br)
{
    struct termios tio;
    ::memset(&tio, 0, sizeof(tio));
    tio.c_iflag = 0;
    tio.c_oflag = 0;
    tio.c_cflag =
        CS8 | CREAD | CLOCAL; // 8n1, see termios.h for more information
    tio.c_lflag = 0;
    tio.c_cc[VMIN] = 1;
    tio.c_cc[VTIME] = 1;

    auto speed = getPosixBaudrate(br);

    serial.cfsetospeed(&tio, speed); // 115200 baud
    serial.cfsetispeed(&tio, speed); // 115200 baud
    return serial.tcsetattr(fd, TCSANOW, &tio);
}

unsigned int
PosixSerialUtils::getPosixBaudrate(Baudrate br)
{
#define CASE(x)            \
    case Baudrate::BR_##x: \
        return B##x;
    switch (br)
    {
        CASE(1200)
        CASE(2400)
        CASE(4800)
        CASE(9600)
        CASE(19200)
        CASE(38400)
        CASE(57600)
        CASE(115200)
        CASE(230400)
        CASE(460800)
    default:
        return -1;
    }
}
