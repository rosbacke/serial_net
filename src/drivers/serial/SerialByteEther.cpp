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

#include "PosixSerialUtils.h"

#include <cerrno>
#include <fcntl.h>
#include <stdexcept>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "hal/PosixIf.h"
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace gsl;

SerialByteEther::SerialByteEther(const std::string& device, SerialHal hal,
                                 RtsOptions rts)
    : m_fd(hal.m_file), m_rxCB(nullptr), m_hal(hal)
{
    LOG_DEBUG << "start setup";
    setup(device);
    setupRts(rts);
    LOG_DEBUG << "end setup";
}

SerialByteEther::~SerialByteEther()
{
}

void
SerialByteEther::registerReadCB(React::MainLoop& mainLoop)
{
    // we'd like to be notified when input is available on stdin
    mainLoop.onReadable(m_fd.get(), [this]() -> bool {
        readSerial();

        // return true, so that we also return future read events
        return true;
    });
}

// Send a byte to the ether.
void
SerialByteEther::sendBytes(const gsl::span<const gsl::byte>& bytes)
{
    auto len = bytes.size();
    int res = m_hal.m_file->write(m_fd, bytes.data(), len);
    if (res == len)
    {
        LOG_TRACE << "Wrote " << len << " byte(s)";
        return;
    }
    LOG_ERROR << "Failed to write all serial bytes.";
    throw std::runtime_error("Failed serial write.");
}

// Register a receiver.
void
SerialByteEther::addClient(ByteEtherIf::RxIf* cb)
{
    m_rxCB = cb;
}

bool
SerialByteEther::readSerial()
{
    gsl::byte rxBuf[256];
    ssize_t size = m_hal.m_file->read(m_fd, &rxBuf, 256);
    if (size > 0 && m_rxCB != nullptr)
    {
        LOG_TRACE << "Read " << size << " bytes.";
        m_rxCB->receiveBytes(gsl::span<byte>(rxBuf, size));
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
    using ps = PosixSerialUtils;

    m_fd.set(m_hal.m_file->open(deviceName.c_str(), O_RDWR | O_NONBLOCK));
    if (m_fd <= 0)
    {
        LOG_ERROR << "Failed to open serial device: " << deviceName
                  << " error:" << strerror(errno);
        throw std::runtime_error("Failed opening serial port.");
    }

    ps::set8N1Termios(*m_hal.m_serial, m_fd, ps::Baudrate::BR_115200);
}

void
SerialByteEther::setupRts(RtsOptions rts)
{
    using ps = PosixSerialUtils;

    switch (rts)
    {
    case RtsOptions::None:
        break;
    case RtsOptions::pulldown:
        ps::setRTS(*m_hal.m_serial, m_fd, ps::IOState::negated);

        // Seems like setting the RTS happens to late and result in a
        // ghost byte being generated. Wait a while to let it settle and read it
        // Should be fine since we do not promise serial services until the
        // setup
        // exits.
        m_hal.m_sleep->usleep(20000);
        uint8_t buf[4];
        m_hal.m_file->read(m_fd, buf, 4);
        break;

    case RtsOptions::rs485_te:
        ps::setRS485Mode(*m_hal.m_serial, m_fd, true, true);
        break;
    }
}
