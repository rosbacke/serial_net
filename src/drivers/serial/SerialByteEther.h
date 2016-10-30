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
 * SerialByteEther.h
 *
 *  Created on: 30 juli 2016
 *      Author: mikaelr
 */

#ifndef SRC_DRIVERS_SERIAL_SERIALBYTEETHER_H_
#define SRC_DRIVERS_SERIAL_SERIALBYTEETHER_H_

#include "interfaces/ByteEtherIf.h"
#include "interfaces/RuntimeIf.h"
#include "reactcpp.h"

#include "../../hal/PosixIf.h"
#include <string>

class PosixFileIf;

/**
 * Implement the driver for a standard Linux serial port for the byte
 * layer.
 */
class SerialByteEther : public ByteEtherIf
{
  public:
    class SerialHal
    {
      public:
        SerialHal(PosixFileIf* file, PosixSleepIf* sleep, PosixSerialIf* serial)
            : m_file(file), m_sleep(sleep), m_serial(serial)
        {
        }
        PosixFileIf* m_file;
        PosixSleepIf* m_sleep;
        PosixSerialIf* m_serial;
    };

    SerialByteEther(const std::string& device, SerialHal hal);
    virtual ~SerialByteEther();

    // Set up callback based reading when data is available.
    void registerReadCB(React::MainLoop& mainLoop);

    // Send a byte to the ether.
    virtual void sendByte(gsl::byte curByte) override;

    // Register a receiver.
    virtual void addClient(ByteEtherIf::RxIf* cb) override;

    int getFd() const
    {
        return m_fd;
    }

  private:
    bool readSerial();

    int setRTS(int fd, int level);

    void setup(const std::string& device);

    int m_fd;
    ByteEtherIf::RxIf* m_rxCB;
    SerialHal m_hal;
};

#endif /* SRC_DRIVERS_SERIAL_SERIALBYTEETHER_H_ */
