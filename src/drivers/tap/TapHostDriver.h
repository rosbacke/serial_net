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
 * TapHostDriver.h
 *
 *  Created on: 23 okt. 2016
 *      Author: mikaelr
 */

#ifndef SRC_DRIVERS_TAP_TAPHOSTDRIVER_H_
#define SRC_DRIVERS_TAP_TAPHOSTDRIVER_H_

#include "TapProtocol.h"
#include "TunTapDriver.h"
#include "eventwrapper/EventLoop.h"
#include "hal/PosixFd.h"
#include "interfaces/MsgHostIf.h"

class PosixFileIf;
class PosixTunTapIf;

class TapHostDriver : public MsgHostIf::RxIf, public MsgHostIf::AddrChange
{
  public:
    TapHostDriver(AddressCache* ac, PosixFileIf* pfi,
                  std::unique_ptr<TunTapDriver>&& ttd);
    virtual ~TapHostDriver();

    void setIfEventCommands(std::string onUp, std::string onDown)
    {
        m_on_if_up = onUp;
        m_on_if_down = onDown;
    }

    void startTransfer(MsgHostIf* txIf, EventLoop& loop);

    /**
     * Called when a packet was received from the serial net.
     */
    virtual void packetReceived(const ByteVec& data, LocalAddress srcAddr,
                                LocalAddress destAddr,
                                ChannelType chType) override
    {
        m_tap.packetReceived(m_tap_fd, data, srcAddr, destAddr);
    }

    static void setTapIfUpDown(PosixFileIf* posixFile, PosixTunTapIf* posixTun,
                               bool up);

    void msgHostRx_newAddr(LocalAddress addr) final;

    void setupCallback(EventLoop& mainLoop);

  private:
    TapProtocol m_tap;

    PosixFd m_tap_fd;
    PosixFileIf* m_pfi;
    std::string m_on_if_up;
    std::string m_on_if_down;
    bool m_ifUp = false;
    std::unique_ptr<TunTapDriver> m_tunTapDriver;
    std::string m_tapName = "tap0";
};

#endif /* SRC_DRIVERS_TAP_TAPHOSTDRIVER_H_ */
