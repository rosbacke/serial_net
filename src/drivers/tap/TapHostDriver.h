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
#include "eventwrapper/EventLoop.h"
#include "hal/PosixFd.h"
#include "interfaces/MsgHostIf.h"

class PosixFileIf;
class PosixTunTapIf;

class TapHostDriver : public MsgHostIf::RxIf, public MsgHostIf::AddrChange
{
  public:
    TapHostDriver(AddressCache* ac, PosixFileIf* pfi, PosixTunTapIf* ptti,
                  const std::string& on_if_up, const std::string& on_if_down);
    virtual ~TapHostDriver();

    void startTransfer(MsgHostIf* txIf, EventLoop& loop);

    /**
     * Called when a packet was received from the serial net.
     */
    virtual void packetReceived(const ByteVec& data, LocalAddress srcAddr,
                                LocalAddress destAddr) override
    {
        m_tap.packetReceived(m_tun_fd, data, srcAddr, destAddr);
    }

    static void setTapIfUpDown(PosixTunTapIf* posix, bool up);

    void msgHostRx_newAddr(LocalAddress addr) final;

  private:
    int tun_alloc(std::string& dev, unsigned tunFlags);

    void setupCallback(EventLoop& mainLoop);
    TapProtocol m_tap;

    PosixFd m_tun_fd;
    PosixFileIf* m_pfi;
    PosixTunTapIf* m_ptti;
    std::string m_on_if_up;
    std::string m_on_if_down;
    bool m_ifUp = false;
};

#endif /* SRC_DRIVERS_TAP_TAPHOSTDRIVER_H_ */
