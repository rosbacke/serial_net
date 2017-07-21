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
 * RealPosixFactoy.cpp
 *
 *  Created on: 29 juni 2017
 *      Author: mikaelr
 */

#include "RealPosixFactory.h"
#include "drivers/serial/SerialByteEther.h"
#include "drivers/tap/TapHostDriver.h"

#include <memory>

RealPosixFactory::RealPosixFactory()
{
    // TODO Auto-generated constructor stub
}

RealPosixFactory::~RealPosixFactory()
{
    // TODO Auto-generated destructor stub
}

std::unique_ptr<SerialByteEther>
RealPosixFactory::makeSerialByteEther(std::string path)
{
    return std::make_unique<SerialByteEther>(path, &m_file, &m_sleep,
                                             &m_serial);
}

std::unique_ptr<TapHostDriver>
RealPosixFactory::makeTapHostDriver(AddressCache* ac)
{
    auto ttd = makeTunTapDriver();
    return std::make_unique<TapHostDriver>(ac, &m_file, std::move(ttd));
}

std::unique_ptr<TunTapDriver>
RealPosixFactory::makeTunTapDriver()
{
    return std::make_unique<TunTapDriver>(&m_file, &m_ptti);
}
