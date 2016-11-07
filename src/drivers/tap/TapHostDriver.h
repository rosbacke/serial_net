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
#include "hal/PosixFd.h"
#include "interfaces/MsgHostIf.h"
#include "reactcpp.h"

class PosixFileIf;
class PosixTunTapIf;

class TapHostDriver : public MsgHostIf
{
  public:
    TapHostDriver(LocalAddress myAddr, AddressCache* ac, PosixFileIf* pfi,
                  PosixTunTapIf* ptti);
    virtual ~TapHostDriver();

    void startTransfer(MsgHostIf::TxIf* txIf, React::Loop& loop);

    /**
     * Called when a packet was received from the serial net.
     */
    virtual void packetReceived(const ByteVec& data, LocalAddress srcAddr,
                                LocalAddress destAddr) override
    {
        m_tap.packetReceived(m_tun_fd, data, srcAddr, destAddr);
    }

    /**
     * Inform the driver where it is supposed to send its packets.
     */
    virtual void setTxHandler(TxIf* txIf) override
    {
        m_tap.setTx(txIf);
    }

  private:
    int tun_alloc(char* dev, unsigned tunFlags);

    void setupCallback(React::Loop& mainLoop);
    TapProtocol m_tap;

    PosixFd m_tun_fd;
    PosixFileIf* m_pfi;
    PosixTunTapIf* m_ptti;
};

#endif /* SRC_DRIVERS_TAP_TAPHOSTDRIVER_H_ */
