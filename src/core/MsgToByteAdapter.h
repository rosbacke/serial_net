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
 * MsgToByteAdapter.h
 *
 *  Created on: 5 aug. 2016
 *      Author: mikaelr
 */

#ifndef SRC_CORE_MSGTOBYTEADAPTER_H_
#define SRC_CORE_MSGTOBYTEADAPTER_H_

#include "../eventwrapper/EventLoop.h"
#include "FrameCodec.h"
#include "interfaces/ByteEtherIf.h"
#include "interfaces/MsgEtherIf.h"

/**
 * Handle the byte to frame encoding/decoding.
 *
 * Sits between the serial driver and the Frame interface.
 */
class MsgToByteAdapter : public MsgEtherIf, public ByteEtherIf::RxIf
{
  public:
    MsgToByteAdapter();
    virtual ~MsgToByteAdapter();

    /// Set execution loop for timeout handling.
    void setExecLoop(EventLoop* loop)
    {
        m_loop = loop;
    }

    // MsgEtherIf methods.
    /// Send a message to the ether.
    virtual void sendMsg(const MsgEtherIf::EtherPkt& msg) override;

    /// Register a receiver.
    virtual void addClient(MsgEtherIf::RxIf* cb) override
    {
        m_codec.setRxCB(cb);
    }

    // Return true when a packet is being received.
    bool packetRxInProgress() override;

    // Implement lower level interface toward the serial port.
    /// Register serial driver to use of rx/tx of the byte stream.
    void setByteIf(ByteEtherIf* beIf);

  private:
    /// Return the current time from react.
    double now() const
    {
        return m_loop->now();
    }

    void checkTimeout();

    void receiveBytes(const gsl::span<gsl::byte>& bytes) override;

    // Receive a byte from the byte interface.
    void newByte(gsl::byte byte);

    ByteEtherIf* m_beIf = nullptr;
    MsgEtherIf::RxIf* m_cb = nullptr;
    FrameCodec m_codec;
    EventLoop* m_loop = nullptr;

    /// Timepoint when the byte was received.
    double m_lastUpdate = 0.0;
};

#endif /* SRC_CORE_MSGTOBYTEADAPTER_H_ */
