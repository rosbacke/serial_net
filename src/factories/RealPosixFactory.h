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
 * RealPosixFactoy.h
 *
 *  Created on: 29 juni 2017
 *      Author: mikaelr
 */

#ifndef SRC_FACTORIES_REALPOSIXFACTORY_H_
#define SRC_FACTORIES_REALPOSIXFACTORY_H_

#include "interfaces/FactoryIf.h"

#include "hal/PosixFileReal.h"
#include "hal/PosixSerialReal.h"
#include "hal/PosixSleepReal.h"
#include "hal/PosixTunTapReal.h"

class RealPosixFactory : public FactoryIf
{
  public:
    RealPosixFactory();
    ~RealPosixFactory();

    std::unique_ptr<SerialByteEther>
    makeSerialByteEther(std::string path) override;

    std::unique_ptr<TapHostDriver> makeTapHostDriver(AddressCache* ac) override;

    std::unique_ptr<TunTapDriver> makeTunTapDriver() override;

    PosixFileIf* getPosixFile() override
    {
        return &m_file;
    }

    PosixFileReal m_file;
    PosixSleepReal m_sleep;
    PosixSerialReal m_serial;
    PosixTunTapReal m_ptti;
};

#endif /* SRC_FACTORIES_REALPOSIXFACTORY_H_ */
