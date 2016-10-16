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
 * SerialNet.h
 *
 *  Created on: 31 juli 2016
 *      Author: mikaelr
 */

#ifndef SRC_MAINS_SERIALNET_SERIALNET_H_
#define SRC_MAINS_SERIALNET_SERIALNET_H_

#include "core/PacketTypeCodec.h"
#include "drivers/serial/SerialByteEther.h"
#include "interfaces/MsgEtherIf.h"
#include "interfaces/RuntimeIf.h"
#include "utility/Config.h"

#include "SNConfig.h"

#include <boost/program_options.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/options_description.hpp>

#include "interfaces/MsgHostIf.h"

#include <memory>

#include <core/AddressCache.h>
#include <drivers/tap/SocatTapHostDriver.h>
#include <reactcpp.h>

namespace po = boost::program_options;

class Master;
class ByteEtherIf;
class MsgEtherIf;
class StdstreamPipeHostDriver;
class SocatTunHostDriver;
class MsgToByteAdapter;

/**
 * Application class for the utility serial net.
 */
class SerialNet
{
  public:
    SerialNet();
    virtual ~SerialNet();

    void start(boost::program_options::variables_map& vm);

    void mainLoop();

    void doExecute();

    void exit();

  private:
    // Current mode of the program.
    SNConfig::Mode m_mode;

    // Vector to schedule processor time.
    std::vector<RuntimeIf*> m_runtime;

    // Interface to the lower level frame interface.
    MsgEtherIf* m_msgEther;

    // Interface to the upper level host interface.
    MsgHostIf* m_msgHost;

    // Configuration from file/various timing constants.

    Config m_config;
    std::unique_ptr<PacketTypeCodec> m_packetTypeCodec;

    std::unique_ptr<StdstreamPipeHostDriver> m_stdHostStdstreamDriver;
    std::unique_ptr<SocatTunHostDriver> m_socatTunHostDriver;
    std::unique_ptr<SocatTapHostDriver> m_socatTapHostDriver;
    std::unique_ptr<AddressCache> m_addressCache;

    std::unique_ptr<MsgToByteAdapter> m_msgToByteAdapter;
    std::unique_ptr<SerialByteEther> m_serialByteEther;

    std::unique_ptr<Master> m_master;

    std::unique_ptr<WSDump> m_wsDump;

    React::MainLoop m_loop;
};

#endif /* SRC_MAINS_SERIALNET_SERIALNET_H_ */
