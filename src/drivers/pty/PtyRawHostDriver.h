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
 * PtyRawHostDriver.h
 *
 *  Created on: 22 juli 2017
 *      Author: mikaelr
 */

#ifndef SRC_DRIVERS_PTY_PTYRAWHOSTDRIVER_H_
#define SRC_DRIVERS_PTY_PTYRAWHOSTDRIVER_H_

#include "eventwrapper/EventLoop.h"
#include "hal/PosixFd.h"
#include "interfaces/MsgHostIf.h"
#include "interfaces/SerialProtocol.h"

#include <iostream>
#include <iterator>

class PosixFileIf;
class FolderManager;

/**
 * Create a virtual terminal (pty) and send data raw channels through
 * it. Allow for simulating serial port connection over the bus given
 * that clients open the pty for access.
 */
class PtyRawHostDriver : public MsgHostIf::RxIf
{
  public:
    PtyRawHostDriver(PosixFileIf* posixIf, EventLoop* loop);
    virtual ~PtyRawHostDriver();
    void setFM(FolderManager* fm)
    {
        m_fm = fm;
    }

    static int openPty();

    static std::string ptyName(int fd);

    /**
     * Called when a packet was received from the serial net.
     */
    void packetReceivedFromNet(const ByteVec& data, LocalAddress srcAddr,
                               LocalAddress destAddr,
                               ChannelType chType) override;

    void setupCallback(MsgHostIf* txIf, LocalAddress peerAddress);

    // Return true if we should continue to monitor read on fd.
    bool doRead();

  private:
    void closePty();

    void triggerPtyRestart();

    void monitorRead();

    PosixFd m_pty_fd;

    LocalAddress m_peerAddr = LocalAddress::null_addr;

    // Where we send our packet for delivery to the ether.
    MsgHostIf* m_txHandler = nullptr;

    EventLoop* m_loop = nullptr;

    FolderManager* m_fm = nullptr;
};

#endif /* SRC_DRIVERS_PTY_PTYRAWHOSTDRIVER_H_ */
