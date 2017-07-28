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
 * TunProtocol.h
 *
 *  Created on: 23 juli 2017
 *      Author: mikaelr
 */

#ifndef SRC_DRIVERS_TUN_TUNPROTOCOL_H_
#define SRC_DRIVERS_TUN_TUNPROTOCOL_H_

#include "interfaces/SerialProtocol.h"

#include <array>

class PosixFileIf;
class MsgHostIf;

class TunProtocol
{
  public:
    TunProtocol(PosixFileIf* pfi) : m_posixFileIf(pfi){};
    virtual ~TunProtocol();

    void setTx(MsgHostIf* txIf)
    {
        m_txIf = txIf;
    }

    void doRead(int fd);

    void doRead2(int fd);

    void packetReceivedFromNet(int fd, const ByteVec& data,
                               LocalAddress srcAddr, LocalAddress destAddr);

  private:
    enum class ReadType
    {
        header,
        data
    };

    MsgHostIf* m_txIf = nullptr;
    PosixFileIf* m_posixFileIf = nullptr;

    // Read up to and including the dest addr of the IPv4.
    static const constexpr int headerLen = 4 + 20;
    std::array<uint8_t, headerLen> m_readHeader;
    ByteVec m_rxTunPacket;

    ReadType m_readType = ReadType::header;
};

#endif /* SRC_DRIVERS_TUN_TUNPROTOCOL_H_ */
