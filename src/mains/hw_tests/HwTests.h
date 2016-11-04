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
 * HwTests.h
 *
 *  Created on: 2 nov. 2016
 *      Author: mikaelr
 */

#ifndef SRC_MAINS_HW_TESTS_HWTESTS_H_
#define SRC_MAINS_HW_TESTS_HWTESTS_H_

#include <string>

class HwTests
{
  public:
    /**
     * Manual test 2016-11-02. System : Linux using USB->RS232 adapter.
     * Measuring on RTSpin using oscilloscope.
     * Default voltage RTS : -6V
     * Set value to 0 -> RTS : -6V
     * Set value to 1 -> RTS : 6V
     *
     * This confirms:
     * 0 <-> negated <-> -6V
     * 1 <-> asserted <-> 6V
     */
    static void doSetRTS(std::string device, int value, int timeout);
};

#endif /* SRC_MAINS_HW_TESTS_HWTESTS_H_ */
