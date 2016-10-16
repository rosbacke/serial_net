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
 * Core.h
 *
 *  Created on: 1 jul 2016
 *      Author: mikaelr
 */

#ifndef SRC_CORE_CORE_H_
#define SRC_CORE_CORE_H_

#include "FrameCodec.h"
#include "interfaces/ByteStreamIf.h"
#include "interfaces/MsgEtherIf.h"
#include <vector>

/**
 * Represent the core functionality of moving packets between the
 * upper and lower interfaces.
 */
class Core : public MsgEtherIf::RxIf
{
  public:
    using DecodeCB = void (*)(const ByteVec&, void* userdata);

    Core(ByteStreamIf* bsif, DecodeCB clientCB, void* cbData);
    virtual ~Core(){};

    /**
     * Send an Ethernet frame to the framework.
     */
    void sendEtherFrame();

    /**
     * Send of a packet to the serial port using 'send_packet' packet type.
     */
    void sendPacketToSerial(int destAddr, int tokenHint, const ByteVec& data);

    /**
     * Receive a number of bytes from the common serial medium.
     */
    void receiveBytes(const ByteVec& bytes);

    /**
     * Extract the ethernet destination MAC and request a local address for it.
     */
    void requestEtherAddr(const ByteVec& packet);

  private:
    void msgEtherRx_newMsg(const ByteVec& msg) override
    {
        receiveSerialPacket(msg);
    }

    /**
     * Receive a decoded packet from the serial net.
     */
    void receiveSerialPacket(const ByteVec& packet);

    ByteStreamIf* m_bsif;
    FrameCodec m_frameCodec;
    DecodeCB m_clientCB;
    void* m_clientCBData;
};

#endif /* SRC_CORE_CORE_H_ */
