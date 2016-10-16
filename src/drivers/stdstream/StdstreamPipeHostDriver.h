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
 * StdstreamOpipeHostDriver.h
 *
 *  Created on: 14 aug. 2016
 *      Author: mikaelr
 */

#ifndef SRC_DRIVERS_STDSTREAM_STDSTREAMPIPEHOSTDRIVER_H_
#define SRC_DRIVERS_STDSTREAM_STDSTREAMPIPEHOSTDRIVER_H_

#include "interfaces/MsgHostIf.h"

#include <iostream>
#include <iterator>
#include <reactcpp.h>

class StdstreamPipeHostDriver : public MsgHostIf
{
  public:
    StdstreamPipeHostDriver(int myAddr);
    virtual ~StdstreamPipeHostDriver();

    void startStdout(int rxAddress);

    void startStdin(int destAddr, TxIf* txIf, React::MainLoop& mainLoop);

    /**
     * Called when a packet was received from the serial net.
     */
    void packetReceived(const ByteVec& data, int srcAddr,
                        int destAddr) override;

  private:
    virtual void setTxHandler(TxIf* txIf) override
    {
        m_txHandler = txIf;
    }

    void setupCallback(React::MainLoop& mainLoop);

    // My address in the network.
    int m_myAddr;

    // Address of the unit we are receiving data from.
    int m_rxAddr;

    // Address we send data to.
    int m_destAddr;

    // Where we send our packet for delivery to the ether.
    MsgHostIf::TxIf* m_txHandler;
};

#endif /* SRC_DRIVERS_STDSTREAM_STDSTREAMPIPEHOSTDRIVER_H_ */
