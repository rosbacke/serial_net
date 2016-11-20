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
 * MsgToByteAdapter.cpp
 *
 *  Created on: 5 aug. 2016
 *      Author: mikaelr
 */

#include "MsgToByteAdapter.h"

#include <reactcpp.h>

MsgToByteAdapter::MsgToByteAdapter()
{
}

MsgToByteAdapter::~MsgToByteAdapter()
{
}

void
MsgToByteAdapter::sendMsg(const EtherPkt& msg)
{
    ByteVec raw;
    m_codec.encodePacket(msg, raw);
    m_beIf->sendBytes(raw);
}

void
MsgToByteAdapter::setByteIf(ByteEtherIf* beIf)
{
    if (m_beIf)
    {
        m_beIf->addClient(nullptr);
    }
    m_beIf = beIf;
    if (m_beIf)
    {
        m_beIf->addClient(this);
    }
}

bool
MsgToByteAdapter::packetRxInProgress()
{
    checkTimeout();
    return m_codec.rxInProgress();
}

void
MsgToByteAdapter::receiveBytes(const gsl::span<gsl::byte>& bytes)
{
    checkTimeout();
    m_lastUpdate = now();
    for (auto data : bytes)
    {
        m_codec.decodeByte(data);
    }
}

void
MsgToByteAdapter::newByte(gsl::byte byte)
{
    checkTimeout();
    m_lastUpdate = now();
    m_codec.decodeByte(byte);
}

void
MsgToByteAdapter::checkTimeout()
{
    if (m_loop && m_codec.rxInProgress())
    {
        auto now_ = now();
        if (now_ - m_lastUpdate > 0.1)
        {
            // Timeout.
            m_codec.reset();
        }
        m_lastUpdate = now_;
    }
}
