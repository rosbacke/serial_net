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
 * EtherIf.h
 *
 *  Created on: 29 jul 2016
 *      Author: mikaelr
 */

#ifndef SRC_CORE_BYTEETHERIF_H_
#define SRC_CORE_BYTEETHERIF_H_

#include "utility/Utility.h"
#include <cstdint>

/**
 * Interface for sending and receiving packets from the low level byte
 * interface.
 */
class ByteEtherIf
{
  public:
    /**
     * Interface for receiving a new byte from the medium.
     */
    class RxIf
    {
      public:
        virtual void receiveBytes(const gsl::span<gsl::byte>& bytes) = 0;
    };

    ByteEtherIf(){};
    virtual ~ByteEtherIf(){};

    // Send a byte to the ether.
    // Will throw if we fail to write data.
    virtual void sendBytes(const gsl::span<const gsl::byte>& bytes) = 0;

    // Register a receiver.
    virtual void addClient(RxIf* cb) = 0;
};

#endif /* SRC_CORE_BYTEETHERIF_H_ */
