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
 * WSDump.h
 *
 *  Created on: 5 okt. 2016
 *      Author: mikaelr
 */

#ifndef SRC_CORE_WSDUMP_H_
#define SRC_CORE_WSDUMP_H_

#include <fstream>
#include <iosfwd>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "interfaces/MsgEtherIf.h"
#include <utility/Utility.h>

/**
 * Implement dumping of packet data to a named pipe suitable for
 * the text2pcap program.
 */
class WSDump
{
  public:
    WSDump(std::string file);

    void rxPacket(const MsgEtherIf::EtherPkt& packet);
    ~WSDump();

  private:
    std::fstream m_fstream;
    std::ostream* m_os;
};

#endif /* SRC_CORE_WSDUMP_H_ */
