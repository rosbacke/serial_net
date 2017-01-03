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
 * PosixFileIf.h
 *
 *  Created on: 29 okt. 2016
 *      Author: mikaelr
 */

#ifndef SRC_UTILITY_POSIXFILEIF_H_
#define SRC_UTILITY_POSIXFILEIF_H_

/**
 * Give an interface for the low level posix operations to allow better testing.
 * Keep same function signature as the original functions. Intended to be as
 * close to the metal as possible.
 * Exception is ioctl which is variadic. Make one function for each used ioctl
 * constant.
 */
#include <sys/types.h>
#include <termios.h>

/**
 * Basic file operations.
 */
class PosixFileIf
{
  public:
    virtual int open(const char* pathname, int flags) = 0;
    virtual int open(const char* pathname, int flags, mode_t mode) = 0;
    virtual ssize_t read(int fd, void* buf, size_t size) const = 0;
    virtual ssize_t write(int fd, const void* buf, size_t size) = 0;
    virtual int close(int fd) = 0;
    virtual int system(const char* cmd) = 0;

  protected:
    ~PosixFileIf(){};
};

/**
 * Sleep functions.
 */
class PosixSleepIf
{
  public:
    virtual int usleep(useconds_t usec) const = 0;

    virtual unsigned int sleep(unsigned int sec) const = 0;

  protected:
    ~PosixSleepIf(){};
};

/**
 * Auxillary serial port functions.
 */
class PosixSerialIf
{
  public:
    virtual int cfsetospeed(struct termios* termios_p, speed_t speed) = 0;
    virtual int cfsetispeed(struct termios* termios_p, speed_t speed) = 0;
    virtual int tcsetattr(int fd, int optional_actions,
                          const struct termios* termios_p) = 0;

    // Note: ioctl uses variadic argument list. Do not work well with
    // inheritance.
    // Use separate functions for each ioctl type.

    /**
     * From https://linux.die.net/man/4/tty_ioctl
     *
     * TIOCMGET,TIOCMSET Modem control lines. get/set bitmask.
     *
     * example of bits:
     * TIOCM_LE        DSR (data set ready/line enable)
     * TIOCM_DTR       DTR (data terminal ready)
     * TIOCM_RTS       RTS (request to send)
     *
     * Setting a bit correspond to outputting an asserted state.
     * (checked by measuring RTS)
     */
    virtual int ioctl_TIOCMGET(int fd, int* status_p) const = 0;
    virtual int ioctl_TIOCMSET(int fd, int* status_p) = 0;

    /**
     * Control RTS level during RS485 mode.
     * Expect a pointer to struct serial_rs485 to be supplied.
     */
    virtual int ioctl_TIOCGRS485(int fd, void* status_p) const = 0;
    virtual int ioctl_TIOCSRS485(int fd, void* status_p) = 0;

  protected:
    ~PosixSerialIf(){};
};

/**
 * Auxillary TUN/TAP  functions.
 */
class PosixTunTapIf
{
  public:
    // Note: ioctl uses variadic argument list. Do not work well with
    // inheritance. Use separate functions for each ioctl type.

    // Set up flags for the TUN interface. Note: ifr_p expected to be
    // a 'struct ifreq *' but according to ref doc, a void * should be fed
    // to the function.
    virtual int ioctl_TUNSETIFF(int fd, void* ifr_p) = 0;

    /**
     * ioctl for accessing netdevice flags.
     */
    virtual int ioctl_SIOCSIFFLAGS(int fd, void* ifr_p) = 0;

    /**
     * ioctl for accessing netdevice flags.
     */
    virtual int ioctl_SIOCGIFFLAGS(int fd, void* ifr_p) = 0;

  protected:
    ~PosixTunTapIf(){};
};

#endif /* SRC_UTILITY_POSIXFILEIF_H_ */
