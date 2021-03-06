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

#ifndef SRC_DRIVERS_STDSTREAM_STDIORAWHOSTDRIVER_H_
#define SRC_DRIVERS_STDSTREAM_STDIORAWHOSTDRIVER_H_

#include "eventwrapper/EventLoop.h"
#include "interfaces/MsgHostIf.h"
#include "interfaces/SerialProtocol.h"
#include <iostream>
#include <iterator>

class PosixFileIf;

class StdioRawHostDriver : public MsgHostIf::RxIf
{
  public:
    StdioRawHostDriver(PosixFileIf* posixIf);
    virtual ~StdioRawHostDriver();

    void startStdout(LocalAddress rxAddress);

    void startStdin(LocalAddress destAddr, MsgHostIf* txIf,
                    EventLoop& mainLoop);

    /**
     * Called when a packet was received from the serial net.
     */
    void packetReceivedFromNet(const ByteVec& data, LocalAddress srcAddr,
                               LocalAddress destAddr,
                               ChannelType chType) override;

  private:
    void writeToNet(MsgHostIf::HostPkt packet);

    void setupCallback(EventLoop& mainLoop);

    // Address of the unit we are receiving data from.
    LocalAddress m_rxAddr;

    // Address we send data to.
    LocalAddress m_destAddr;

    // Where we send our packet for delivery to the ether.
    MsgHostIf* m_txHandler;

    // Interface for the posix filer operations.
    PosixFileIf* m_posixIf;
};

#endif /* SRC_DRIVERS_STDSTREAM_STDIORAWHOSTDRIVER_H_ */
