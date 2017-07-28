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
 * TunHostDriver.h
 *
 *  Created on: 23 juli 2017
 *      Author: mikaelr
 */

#ifndef SRC_DRIVERS_TUN_TUNHOSTDRIVER_H_
#define SRC_DRIVERS_TUN_TUNHOSTDRIVER_H_

#include "TunProtocol.h"
#include "drivers/tap/TunTapDriver.h"
#include "eventwrapper/EventLoop.h"
#include "hal/PosixFd.h"
#include "interfaces/MsgHostIf.h"

class PosixFileIf;
class PosixTunTapIf;

class TunHostDriver : public MsgHostIf::RxIf, public MsgHostIf::AddrChange
{
  public:
    TunHostDriver(PosixFileIf* pfi, std::unique_ptr<TunTapDriver>&& ttd);

    virtual ~TunHostDriver();

    void setIfEventCommands(std::string onUp, std::string onDown)
    {
        m_on_if_up = onUp;
        m_on_if_down = onDown;
    }

    void startTransfer(MsgHostIf* txIf, EventLoop& loop);

    /**
     * Called when a packet was received from the serial net.
     */
    virtual void packetReceivedFromNet(const ByteVec& data,
                                       LocalAddress srcAddr,
                                       LocalAddress destAddr,
                                       ChannelType chType) override
    {
        if (chType == ChannelType::tun_format)
        {
            m_tun.packetReceivedFromNet(m_tun_fd, data, srcAddr, destAddr);
        }
        else
        {
            throw std::runtime_error(
                "trying to decode tun packet with wrong packet type.");
        }
    }

    static void setTunIfUpDown(PosixFileIf* posixFile, PosixTunTapIf* posixTun,
                               bool up);

    void msgHostRx_newAddr(LocalAddress addr) final;

    void setupCallback(EventLoop& mainLoop);

  private:
    PosixFd m_tun_fd;
    PosixFileIf* m_pfi;
    TunProtocol m_tun;

    std::string m_on_if_up;
    std::string m_on_if_down;
    bool m_ifUp = false;
    std::unique_ptr<TunTapDriver> m_tunTapDriver;
    std::string m_tunName = "tun0";
};

#endif /* SRC_DRIVERS_TUN_TUNHOSTDRIVER_H_ */
