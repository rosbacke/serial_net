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
 * ByteStreamIf.h
 *
 *  Created on: 1 jul 2016
 *      Author: mikaelr
 */

#ifndef SRC_CORE_BYTESTREAMIF_H_
#define SRC_CORE_BYTESTREAMIF_H_

#include <cstdint>
#include <vector>

#include "utility/Utility.h"

/**
 * Basic interface that a driver implementing the bottom part access to
 * the shared media should implement. It assumes that all bytes written
 * will be distributed on the common medium. It also assumes all bytes
 * written will be reflected back as incoming bytes in the read part.
 */
class ByteStreamIf
{
  public:
    virtual void write(const ByteVec& data) = 0;
    virtual ~ByteStreamIf(){};
};

#endif /* SRC_CORE_BYTESTREAMIF_H_ */
