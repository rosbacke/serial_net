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
 * PosixSerialUtils.h
 *
 *  Created on: 2 nov. 2016
 *      Author: mikaelr
 */

#ifndef SRC_DRIVERS_SERIAL_POSIXSERIALUTILS_H_
#define SRC_DRIVERS_SERIAL_POSIXSERIALUTILS_H_

class PosixSerialIf;

/**
 * Helper class performing various low level access to the Posix
 * serial driver.
 */
class PosixSerialUtils
{
  public:
    enum class IOState
    {
        negated,
        asserted
    };

    enum class Baudrate
    {
        BR_1200,
        BR_2400,
        BR_4800,
        BR_9600,
        BR_19200,
        BR_38400,
        BR_57600,
        BR_115200,
        BR_230400,
        BR_460800
    };

    /**
     * Set up a standard 8N1 serial setting using termios.
     */
    static int set8N1Termios(PosixSerialIf& serial, int fd,
                             Baudrate br = Baudrate::BR_115200);

    /**
     * Given an open file descriptor. Take control of the RTS line
     * and set it. For standard RS232, assert -> +12V signal, negate -> -12V
     * signal.
     *
     * @param serial Interface to the OS level serial functions.
     * @param fd an active file descriptor.
     * @param state : Desired state of the line.
     *
     * @return result of ioctl.
     */
    static int setRTS(PosixSerialIf& serial, int fd, IOState state);

    /**
     * The RS485 mode for Linux serial ports uses the RTS pin to emit
     * a signal suitable for controlling the Drive Enable signal on the
     * RS485 driver circuit.
     *
     * @param serial Interface to the OS level serial functions.
     * @param fd an active file descriptor.
     * @param enable true if we want the RTS to implement the RS485 enable
     * signal.
     * @param rxEnabledDuringTx If the bytes sent out on the bus should be
     * received also.
     * @param duringTx State of the RTS pin while TX is under way.
     * @param afterTx State of the RTS pin after TX is done.
     *
     * @return result of ioctl.
     */
    static int setRS485Mode(PosixSerialIf& serial, int fd, bool enable,
                            bool rxEnabledDuringTx,
                            IOState duringTx = IOState::asserted,
                            IOState afterTx = IOState::negated);

    /**
     * Given a baudrate, return posix specified integer for termios.
     */
    static unsigned int getPosixBaudrate(Baudrate br);
};

#endif /* SRC_DRIVERS_SERIAL_POSIXSERIALUTILS_H_ */
