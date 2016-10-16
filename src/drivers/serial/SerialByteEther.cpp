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
 * SerialByteEther.cpp
 *
 *  Created on: 30 juli 2016
 *      Author: mikaelr
 */

#include "SerialByteEther.h"

#include "utility/Log.h"
#include "utility/Utility.h"

#include <cerrno>
#include <fcntl.h>
#include <stdexcept>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace gsl;

SerialByteEther::SerialByteEther(const std::string& device)
    : m_fd(-1), m_rxCB(nullptr)
{
    LOG_DEBUG << "start setup";
    setup(device);
    LOG_DEBUG << "end setup";
}

SerialByteEther::~SerialByteEther()
{
    if (m_fd != -1)
    {
        ::close(m_fd);
    }
}

void
SerialByteEther::registerReadCB(React::MainLoop& mainLoop)
{
    // we'd like to be notified when input is available on stdin
    mainLoop.onReadable(m_fd, [this]() -> bool {
        readSerial();

        // return true, so that we also return future read events
        return true;
    });
}

// Send a byte to the ether.
void
SerialByteEther::sendByte(byte myByte)
{
    uint8_t tmp = to_integer<uint8_t>(myByte);
    int res = ::write(m_fd, &tmp, 1);
    if (res == 1)
    {
        LOG_TRACE << "Wrote 1 byte:" << +tmp;
        return;
    }
    LOG_ERROR << "Failed to write serial byte.";
    throw std::runtime_error("Failed serial write.");
}

// Register a receiver.
void
SerialByteEther::addClient(ByteEtherIf::RxIf* cb)
{
    m_rxCB = cb;
}

bool
SerialByteEther::execute()
{
    readSerial();
    return false;
}

bool
SerialByteEther::readSerial()
{
    uint8_t rxBuf[256];
    ssize_t size = ::read(m_fd, &rxBuf, 256);
    if (size > 0 && m_rxCB != nullptr)
    {
        LOG_TRACE << "Read " << size << " bytes.";
        for (int i = 0; i < size; ++i)
        {
            m_rxCB->newByte(to_byte(rxBuf[i]));
        }
        return true;
    }
    int myErrno = errno;
    if (size == -1 && (myErrno == EAGAIN || myErrno == EWOULDBLOCK))
    {
        return false;
    }
    LOG_ERROR << "Unexpected errno value in serial read:" << myErrno;
    throw std::runtime_error("Unexpected value in serial read");
}

void
SerialByteEther::setup(const std::string& deviceName)
{
    struct termios tio;

    ::memset(&tio, 0, sizeof(tio));
    tio.c_iflag = 0;
    tio.c_oflag = 0;
    tio.c_cflag =
        CS8 | CREAD | CLOCAL; // 8n1, see termios.h for more information
    tio.c_lflag = 0;
    tio.c_cc[VMIN] = 1;
    tio.c_cc[VTIME] = 5;

    m_fd = ::open(deviceName.c_str(), O_RDWR | O_NONBLOCK);
    if (m_fd <= 0)
    {
        LOG_ERROR << "Failed to open serial device: " << deviceName
                  << " error:" << strerror(errno);
        throw std::runtime_error("Failed opening serial port.");
    }
    ::cfsetospeed(&tio, B115200); // 115200 baud
    ::cfsetispeed(&tio, B115200); // 115200 baud
    ::tcsetattr(m_fd, TCSANOW, &tio);
    setRTS(m_fd, 0);

    // Seems like setting the RTS happens to late and result in a
    // ghost byte being generated. Wait a while to let it settle and read it
    // Should be fine since we do not promise serial services until the setup
    // exits.
    usleep(20000);
    uint8_t buf[4];
    ::read(m_fd, buf, 4);
}

int
SerialByteEther::setRTS(int fd, int level)
{
    int status;

    if (ioctl(fd, TIOCMGET, &status) == -1)
    {
        perror("setRTS(): TIOCMGET");
        return 0;
    }
    if (level)
        status |= TIOCM_RTS;
    else
        status &= ~TIOCM_RTS;
    if (ioctl(fd, TIOCMSET, &status) == -1)
    {
        perror("setRTS(): TIOCMSET");
        return 0;
    }
    return 1;
}
