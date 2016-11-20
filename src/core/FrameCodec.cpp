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
 * FrameCodec.cpp
 *
 *  Created on: 1 jul 2016
 *      Author: mikaelr
 */

#include "FrameCodec.h"
#include "utility/Log.h"
#include "utility/Utility.h"

#include <algorithm>
#include <vector>

using gsl::byte;
using gsl::to_byte;
using gsl::to_integer;

FrameCodec::FrameCodec(MsgEtherIf::RxIf* decodeCB)
    : m_decodeCB(decodeCB), m_state(DecodeState::getKW), m_length(0),
      m_index(0), m_checksum(to_byte<0>())
{
}

FrameCodec::~FrameCodec()
{
}

void
FrameCodec::encodePacket(const MsgEtherIf::EtherPkt& data, ByteVec& result)
{
    const std::size_t dataLen = data.size();
    auto checksum = to_byte<0>();

    result.resize(dataLen + 5);
    result[0] = keyword;
    checksum ^= result[0];
    auto offset = 0_sz;
    if (dataLen < 0x80)
    {
        result.resize(dataLen + 4);
        result[1] = to_byte(static_cast<uint8_t>(dataLen));
        offset = 2;
        checksum ^= result[1];
    }
    else
    {
        offset = 3;
        result[1] =
            to_byte(static_cast<uint8_t>(((dataLen >> 7) & 0x7f) | 0x80));
        checksum ^= result[1];
        result[2] = to_byte(static_cast<uint8_t>(dataLen & 0x7f));
        checksum ^= result[2];
    }
    std::copy(data.begin(), data.end(), result.begin() + offset);
    for (auto el : data)
    {
        checksum ^= el;
    }
    result[dataLen + offset] = checksum;
    result[dataLen + offset + 1] = to_byte<0xff>();
}

void
FrameCodec::receiveData(const ByteVec& data)
{
    for (auto i : data)
        decodeByte(i);
}

void
FrameCodec::decodeByte(byte curByte)
{
    const auto byteAsUInt = to_integer<unsigned>(curByte);
    switch (m_state)
    {
    case DecodeState::getKW:
    {
        if (curByte == keyword)
        {
            m_checksum = keyword;
            m_state = DecodeState::getLen;
            m_length = 0;
        }
        break;
    }
    case DecodeState::getLen:
    {
        m_checksum ^= curByte;
        if ((byteAsUInt & 0x80) != 0)
        {
            m_length |= byteAsUInt & 0x7f;
            m_length <<= 7;
        }
        else
        {
            m_length |= byteAsUInt & 0x7f;
            m_index = 0;
            m_state = DecodeState::getData;
            m_decodePacket.resize(m_length);
        }
        break;
    }
    case DecodeState::getData:
    {
        m_checksum ^= curByte;
        m_decodePacket[m_index++] = curByte;
        if (m_index == m_length)
        {
            m_state = DecodeState::getChecksum;
        }
        break;
    }
    case DecodeState::getChecksum:
    {
        m_checksum ^= curByte;
        m_state = DecodeState::getFiller;
        break;
    }
    case DecodeState::getFiller:
    {
        if (m_checksum == to_byte<0>() && m_decodeCB)
        {
            MsgEtherIf::EtherPkt pt(m_decodePacket.data(), m_length);
            m_decodeCB->msgEtherRx_newMsg(pt);
        }
        else
        {
            LOG_WARN << "Bad checksum of packet.";
        }
        m_state = DecodeState::getKW;
        break;
    }
    }
}
