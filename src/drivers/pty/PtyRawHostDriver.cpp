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
 * PtyRawHostDriver.cpp
 *
 *  Created on: 22 juli 2017
 *      Author: mikaelr
 */

#include "PtyRawHostDriver.h"

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include <errno.h>
#include <sstream>
#include <string.h>

#include <iostream>

#include "hal/PosixIf.h"
#include "interfaces/SerialProtocol.h"
#include "string.h"
#include "utility/Log.h"
#include <cerrno>

#include "core/FolderManager.h"

using namespace std;
using gsl::byte;

PtyRawHostDriver::PtyRawHostDriver(PosixFileIf* posixIf, EventLoop* loop)
    : m_pty_fd(posixIf), m_loop(loop)
{
}

PtyRawHostDriver::~PtyRawHostDriver()
{
    closePty();
}

int
PtyRawHostDriver::openPty()
{
    int fd = ::posix_openpt(O_RDWR | O_NOCTTY);
    // TODO: Decide if O_NOCTTY is needed.
    if (fd == -1)
    {
        std::stringstream ss;
        ss << "Failed posix_openpt. Error:" << ::strerror(errno);
        throw std::runtime_error(ss.str());
    }
    int res = ::grantpt(fd);
    if (res == -1)
    {
        std::stringstream ss;
        ss << "Failed grantpt. Error:" << ::strerror(errno);
        throw std::runtime_error(ss.str());
    }
    res = ::unlockpt(fd);
    if (res == -1)
    {
        std::stringstream ss;
        ss << "Failed unlockpt. Error:" << ::strerror(errno);
        throw std::runtime_error(ss.str());
    }

    // Ok, got pty, remove line editing. Normally not desired.
    // and will result in echo loops under normal programs.
    struct termios ti;
    ::memset(&ti, 0, sizeof ti);
    res = ::tcgetattr(fd, &ti);
    if (res == -1)
        throw std::system_error(errno, std::system_category());
    ::cfmakeraw(&ti);
    res = ::tcsetattr(fd, TCSANOW, &ti);
    if (res == -1)
        throw std::system_error(errno, std::system_category());

    return fd;
}

std::string
PtyRawHostDriver::ptyName(int fd)
{
    const char* name = ::ptsname(fd);
    if (name == nullptr)
        throw std::runtime_error("No pty name given");
    return std::string(name);
}

void
PtyRawHostDriver::packetReceivedFromNet(const ByteVec& data,
                                        LocalAddress srcAddr,
                                        LocalAddress destAddr,
                                        ChannelType chType)
{
    if (chType == ChannelType::raw_stream)
    {
        if (srcAddr == m_peerAddr || m_peerAddr == LocalAddress::broadcast)
        {
            auto posixIf = m_pty_fd.posixIf();
            posixIf->write(m_pty_fd, &data[0], data.size());
        }
    }
}

bool
PtyRawHostDriver::doRead()
{
    const int buffLen = 2048;
    std::array<byte, buffLen> readBuf;
    auto posixIf = m_pty_fd.posixIf();
    int readlen = posixIf->read(m_pty_fd, readBuf.data(), buffLen);

    if (readlen == -1)
    {
        if (errno == EIO)
        {
            // This will happen when the pty slave side has unexpectedly died.
            // For example, doing a 'cat' in a terminal window and pressing
            // ctrl-c.
            // This should not result in this side crashing. However, the pty is
            // gone
            // so another user can't open it.
            // Hence, need to restart the pty.
            triggerPtyRestart();
            return false;
        }
        LOG_INFO << "Read -1, errno:" << errno
                 << " meaning:" << ::strerror(errno);
    }
    else
    {
        LOG_DEBUG << "PTY read length: " << readlen;
        if (m_txHandler)
        {
            MsgHostIf::HostPkt hostPkt(readBuf.data(), readlen);
            m_txHandler->msgHostTx_sendPacket(hostPkt, m_peerAddr,
                                              ChannelType::raw_stream);
        }
    }
    return true;
}

void
PtyRawHostDriver::closePty()
{
    if (m_fm && m_pty_fd.valid())
        m_fm->removePtyLink(m_peerAddr);
    m_pty_fd.close();
}

void
PtyRawHostDriver::triggerPtyRestart()
{
    m_loop->onAvailable([this]() -> bool {
        closePty();
        monitorRead();
        return false;
    });
}

void
PtyRawHostDriver::setupCallback(MsgHostIf* txIf, LocalAddress peerAddress)
{
    m_peerAddr = peerAddress;
    m_txHandler = txIf;
    monitorRead();
    m_txHandler->setRxHandler(this, ChannelType::raw_stream);
}

void
PtyRawHostDriver::monitorRead()
{
    m_pty_fd.set(openPty());
    auto name = ptyName(m_pty_fd);
    LOG_INFO << "Pty name: " << name;
    if (m_fm)
        m_fm->addPtyLink(m_peerAddr, name);

    // we'd like to be notified when input is available to read.
    m_loop->onReadable(m_pty_fd, [this]() -> bool {
        LOG_DEBUG << "Read from pty.";
        auto continueToMonitor = doRead();

        return continueToMonitor;
    });
}
