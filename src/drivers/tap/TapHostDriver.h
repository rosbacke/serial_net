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
#include "interfaces/MsgHostIf.h"
#include "reactcpp.h"

class TapHostDriver : public MsgHostIf
{
  public:
    TapHostDriver(int myAddr, AddressCache* ac);
    virtual ~TapHostDriver();

    void startTransfer(MsgHostIf::TxIf* txIf, React::Loop& loop);

    /**
     * Called when a packet was received from the serial net.
     */
    virtual void packetReceived(const ByteVec& data, int srcAddr,
                                int destAddr) override
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
    void setupCallback(React::Loop& mainLoop);
    TapProtocol m_tap;

    int m_tun_fd;
};

#endif /* SRC_DRIVERS_TAP_TAPHOSTDRIVER_H_ */
