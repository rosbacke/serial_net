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
#include "interfaces/SerialProtocol.h"

#include <iostream>
#include <iterator>
#include <reactcpp.h>

class PosixFileIf;

class StdstreamPipeHostDriver : public MsgHostIf::RxIf
{
  public:
    StdstreamPipeHostDriver(LocalAddress myAddr, PosixFileIf* posixIf);
    virtual ~StdstreamPipeHostDriver();

    void startStdout(LocalAddress rxAddress);

    void startStdin(LocalAddress destAddr, MsgHostIf* txIf,
                    React::MainLoop& mainLoop);

    /**
     * Called when a packet was received from the serial net.
     */
    void packetReceived(const ByteVec& data, LocalAddress srcAddr,
                        LocalAddress destAddr) override;

  private:
    void setupCallback(React::MainLoop& mainLoop);

    // My address in the network.
    LocalAddress m_myAddr;

    // Address of the unit we are receiving data from.
    LocalAddress m_rxAddr;

    // Address we send data to.
    LocalAddress m_destAddr;

    // Where we send our packet for delivery to the ether.
    MsgHostIf* m_txHandler;

    // Interface for the posix filer operations.
    PosixFileIf* m_posixIf;
};

#endif /* SRC_DRIVERS_STDSTREAM_STDSTREAMPIPEHOSTDRIVER_H_ */
