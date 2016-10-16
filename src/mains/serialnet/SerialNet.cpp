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
 * SerialNet.cpp
 *
 *  Created on: 31 juli 2016
 *      Author: mikaelr
 */

#include "SerialNet.h"

#include "interfaces/ByteEtherIf.h"
#include "interfaces/MsgEtherIf.h"
#include "interfaces/MsgHostIf.h"
#include "utility/Log.h"

#include "core/MsgToByteAdapter.h"
#include "core/WSDump.h"
#include "drivers/serial/SerialByteEther.h"
#include "master/Master.h"

#include <boost/program_options.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/options_description.hpp>
#include <drivers/tap/SocatTapHostDriver.h>

#include "drivers/stdstream/StdstreamPipeHostDriver.h"
#include "drivers/tun/SocatTunHostDriver.h"

SerialNet::SerialNet()
    : m_mode(SNConfig::Mode::unknown), m_msgEther(nullptr), m_msgHost(nullptr)
{
}

SerialNet::~SerialNet()
{
}

void
SerialNet::start(boost::program_options::variables_map& vm)
{
    int destAddr = 255;
    std::string device = vm["serial_device"].as<std::string>();
    std::string mode = vm["mode"].as<std::string>();

    if (vm.count("wsdump") > 0)
    {
        std::string fname = vm["wsdump"].as<std::string>();
        m_wsDump = std::make_unique<WSDump>(fname);
    }

    m_mode = SNConfig::toMode(mode);
    int myAddr = vm["address"].as<int>();

    LOG_INFO << "mode: " << SNConfig::toString(m_mode);
    LOG_INFO << "addr: " << myAddr;

    if (m_mode == SNConfig::Mode::std_in)
    {
        if (vm.count("dest_address") != 1)
        {
            LOG_ERROR << "Need a destination address for mode std_in.";
            throw std::runtime_error("destination address.");
        }
        else
        {
            destAddr = vm["dest_address"].as<int>();
        }
    }

    LOG_INFO << "Got dest addr: " << destAddr;

    // Hard code serial interface initially.
    m_serialByteEther = std::make_unique<SerialByteEther>(device);
    m_serialByteEther->registerReadCB(m_loop);

    m_msgToByteAdapter = std::make_unique<MsgToByteAdapter>();
    m_msgToByteAdapter->setByteIf(m_serialByteEther.get());
    m_msgToByteAdapter->setExecLoop(m_loop);

    m_msgEther = static_cast<MsgEtherIf*>(m_msgToByteAdapter.get());

    m_packetTypeCodec = std::make_unique<PacketTypeCodec>(m_msgEther, myAddr);

    m_msgEther->addClient(m_packetTypeCodec.get());

    const bool usePipeHostDriver =          //
        m_mode == SNConfig::Mode::std_out   //
        || m_mode == SNConfig::Mode::std_in //
        || m_mode == SNConfig::Mode::std_io;

    if (usePipeHostDriver)
    {
        m_stdHostStdstreamDriver =
            std::make_unique<StdstreamPipeHostDriver>(myAddr);
        switch (m_mode)
        {
        case SNConfig::Mode::std_out:
            m_stdHostStdstreamDriver->startStdout(255);
            break;

        case SNConfig::Mode::std_in:
            m_stdHostStdstreamDriver->startStdin(
                destAddr, m_packetTypeCodec.get(), m_loop);
            break;

        case SNConfig::Mode::std_io:
            m_stdHostStdstreamDriver->startStdout(255);
            m_stdHostStdstreamDriver->startStdin(
                destAddr, m_packetTypeCodec.get(), m_loop);
            break;

        default:
            break;
        }
        m_msgHost = m_stdHostStdstreamDriver.get();
    }
    else if (m_mode == SNConfig::Mode::socat_tun)
    {
        m_socatTunHostDriver = std::make_unique<SocatTunHostDriver>(myAddr);
        m_socatTunHostDriver->startTransfer(m_packetTypeCodec.get(), m_loop);
        m_msgHost = m_socatTunHostDriver.get();
    }
    else if (m_mode == SNConfig::Mode::socat_tap)
    {
        m_addressCache = std::make_unique<AddressCache>();
        m_socatTapHostDriver = std::make_unique<SocatTapHostDriver>(myAddr);
        m_socatTapHostDriver->setAddressCache(m_addressCache.get());
        m_packetTypeCodec->setAddressCache(m_addressCache.get());
        m_socatTapHostDriver->startTransfer(m_packetTypeCodec.get(), m_loop);

        m_msgHost = m_socatTapHostDriver.get();
    }
    m_packetTypeCodec->setHostIf(m_msgHost);
    m_packetTypeCodec->setWsDump(m_wsDump.get());

    const bool startMaster = vm.count("master") > 0;
    if (startMaster)
    {
        LOG_INFO << "Starting master.";
        m_master = std::make_unique<Master>(m_loop, m_packetTypeCodec.get(),
                                            myAddr, &m_config);

        const int masterTimeout = vm["mtimeout"].as<int>();
        if (masterTimeout > -1)
        {
            // Timestamp timeout, const TimeoutCallback &callback)
            m_loop.onTimeout(masterTimeout, [&]() -> bool {
                // report that we got a signal
                std::cerr << "Timeout on master" << std::endl;

                m_master->exitMaster();
                this->m_master.reset();
                return false;
            });
        }
    }
    m_packetTypeCodec->setMasterEndedCB([&]() {
        LOG_INFO << "Received master_stop. Quict client.";
        m_loop.stop();
    });
}

void
SerialNet::exit()
{
    m_loop.stop();
}

void
SerialNet::mainLoop()
{
    // handler when control+c is pressed
    m_loop.onSignal(SIGINT, [&]() -> bool {
        // report that we got a signal
        std::cerr << "control+c detected" << std::endl;

        // stop the application
        m_loop.stop();

        // although this code is unreachable, we return false because
        // we're no longer interested in future SIGINT signals
        return false;
    });

    // run the event loop
    m_loop.run();
}
