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
 * FactoryIf.h
 *
 *  Created on: 29 juni 2017
 *      Author: mikaelr
 */

#ifndef SRC_INTERFACES_FACTORYIF_H_
#define SRC_INTERFACES_FACTORYIF_H_

#include <memory>

class SerialByteEther;
class TapHostDriver;
class AddressCache;
class TunTapDriver;
class SNConfig;
class PosixFileIf;

class FactoryIf
{
  public:
    FactoryIf(){};

    virtual std::unique_ptr<SerialByteEther>
    makeSerialByteEther(std::string path) = 0;

    virtual std::unique_ptr<TapHostDriver>
    makeTapHostDriver(AddressCache* ac) = 0;

    virtual std::unique_ptr<TunTapDriver> makeTunTapDriver() = 0;

    virtual PosixFileIf* getPosixFile() = 0;

  protected:
    virtual ~FactoryIf(){};
};

#endif /* SRC_INTERFACES_FACTORYIF_H_ */
