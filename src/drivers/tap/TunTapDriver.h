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
 * TunTapDriver.h
 *
 *  Created on: 4 juli 2017
 *      Author: mikaelr
 */

#ifndef SRC_DRIVERS_TAP_TUNTAPDRIVER_H_
#define SRC_DRIVERS_TAP_TUNTAPDRIVER_H_

#include <string>

class PosixFileIf;
class PosixTunTapIf;

class TunTapDriver
{
  public:
    enum class IfType
    {
        tun,
        tap
    };
    TunTapDriver(PosixFileIf* pfi, PosixTunTapIf* ptti);
    ~TunTapDriver();

    int tuntap_alloc(std::string& dev, IfType type = IfType::tap);

    void setUserGroup(int fd, std::string user = "", std::string group = "");

    void setIfUpDown(bool up, std::string tapName);

    void persist(int fd, bool persist);

  private:
    PosixFileIf* m_pfi;
    PosixTunTapIf* m_ptti;
};

#endif /* SRC_DRIVERS_TAP_TUNTAPDRIVER_H_ */
