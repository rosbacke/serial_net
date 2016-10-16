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
 * ProgramVersion.h
 *
 *  Created on: 13 aug. 2016
 *      Author: mikaelr
 */

#ifndef SRC_UTILITY_PROGRAMVERSION_H_
#define SRC_UTILITY_PROGRAMVERSION_H_

#include <string>

class ProgramVersion
{
  public:
    ProgramVersion();
    ~ProgramVersion();

    static std::string getVersionString();
};

#endif /* SRC_UTILITY_PROGRAMVERSION_H_ */
