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
 * FrameCodec.h
 *
 *  Created on: 1 jul 2016
 *      Author: mikaelr
 */

#ifndef SRC_CORE_FRAMECODEC_H_
#define SRC_CORE_FRAMECODEC_H_

#include "gsl/gsl"
#include "interfaces/MsgEtherIf.h"
#include "utility/Utility.h"

#include <cstdint>
#include <vector>

/**
 * Responsible for encoding and decoding the frame format of the serial
 * medium packets.
 */
class FrameCodec
{
    static constexpr auto keyword = gsl::to_byte<0xfd>();

  public:
    /**
     * Constructor
     * @param decodeCB Use to report the newly received packet.
     */
    FrameCodec(MsgEtherIf::RxIf* decodeCB = nullptr);
    ~FrameCodec();

    /**
     * Receive a number of bytes from the serial network.
     * Run through decoding FSM and call packet CB as needed.
     */
    void receiveData(const ByteVec& data);

    /**
     * Encode a given packet with the frame format.
     */
    void encodePacket(const ByteVec& data, ByteVec& result);

    /**
     * Add bytes to be decoded.
     */
    void decodeByte(gsl::byte byte);

    /// Set the callback to report new packets to.
    void setRxCB(MsgEtherIf::RxIf* rxCB)
    {
        m_decodeCB = rxCB;
    }

    /// Return true if a packet is in the process of being received.
    bool rxInProgress() const
    {
        return m_state != DecodeState::getKW;
    }

    void reset()
    {
        m_state = DecodeState::getKW;
    }

  private:
    enum class DecodeState
    {
        getKW,
        getLen,
        getData,
        getChecksum,
        getFiller
    };
    MsgEtherIf::RxIf* m_decodeCB;

    ByteVec m_decodePacket;
    DecodeState m_state;
    std::size_t m_length;
    std::size_t m_index;

    gsl::byte m_checksum;
};

#endif /* SRC_CORE_FRAMECODEC_H_ */
