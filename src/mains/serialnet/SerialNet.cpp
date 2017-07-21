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

#include "core/PacketTypeCodec.h"
#include "core/TxQueue.h"

#include "core/MsgToByteAdapter.h"
#include "core/WSDump.h"

#include "drivers/serial/SerialByteEther.h"
#include "master/Master.h"

#include "drivers/stdstream/StdstreamPipeHostDriver.h"
#include "drivers/tap/SocatTapHostDriver.h"
#include "drivers/tap/TapHostDriver.h"
#include "drivers/tun/SocatTunHostDriver.h"

#include "factories/RealPosixFactory.h"

#include <boost/program_options.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/options_description.hpp>

SerialNet::SerialNet() : m_ts(m_loop)
{
}

SerialNet::~SerialNet()
{
}

bool
SerialNet::start()
{
    m_factory = std::make_unique<RealPosixFactory>();

    if (m_snConfig.m_wsDumpPath != "")
    {
        m_wsDump = std::make_unique<WSDump>(m_snConfig.m_wsDumpPath);
    }

    const auto mode = m_snConfig.m_mode;
    if (mode == SNConfig::Mode::setup_tap)
    {
        // Set up a persistent tun/tap network driver
        PosixFd fd(m_factory->getPosixFile());
        auto ttd = m_factory->makeTunTapDriver();
        fd.set(
            ttd->tuntap_alloc(m_snConfig.m_tapName, TunTapDriver::IfType::tap));
        ttd->setUserGroup(fd, m_snConfig.m_user, m_snConfig.m_group);
        ttd->persist(fd, 1);

        return false;
    }

    if (mode == SNConfig::Mode::remove_tap)
    {
        // Set up a persistent tun/tap network driver
        PosixFd fd(m_factory->getPosixFile());
        auto ttd = m_factory->makeTunTapDriver();
        fd.set(
            ttd->tuntap_alloc(m_snConfig.m_tapName, TunTapDriver::IfType::tap));
        ttd->persist(fd, 0);

        return false;
    }

    m_serialByteEther = m_factory->makeSerialByteEther(m_snConfig.m_etherPath);
    m_serialByteEther->setupRts(m_snConfig.m_rtsOption);
    m_serialByteEther->registerReadCB(&m_loop);

    m_msgToByteAdapter = std::make_unique<MsgToByteAdapter>(m_ts);
    m_msgToByteAdapter->setByteIf(m_serialByteEther.get());

    m_msgEther = m_msgToByteAdapter.get();

    m_txQueue = std::make_unique<TxQueue>(m_msgEther, m_snConfig.m_staticAddr);
    m_packetTypeCodec =
        std::make_unique<PacketTypeCodec>(m_msgEther, m_txQueue.get());

    m_msgEther->addClient(m_packetTypeCodec.get());

    const bool usePipeHostDriver =        //
        mode == SNConfig::Mode::std_out   //
        || mode == SNConfig::Mode::std_in //
        || mode == SNConfig::Mode::std_io;

    if (usePipeHostDriver)
    {
        m_stdHostStdstreamDriver = std::make_unique<StdstreamPipeHostDriver>(
            m_snConfig.m_staticAddr, &m_posixFileIf);
        switch (mode)
        {
        case SNConfig::Mode::std_out:
            m_stdHostStdstreamDriver->startStdout(LocalAddress::broadcast);
            break;

        case SNConfig::Mode::std_in:
            m_stdHostStdstreamDriver->startStdin(m_snConfig.m_destAddr,
                                                 m_txQueue.get(), m_loop);
            break;

        case SNConfig::Mode::std_io:
            m_stdHostStdstreamDriver->startStdout(LocalAddress::broadcast);
            m_stdHostStdstreamDriver->startStdin(m_snConfig.m_destAddr,
                                                 m_txQueue.get(), m_loop);
            break;

        default:
            break;
        }
    }
    else if (mode == SNConfig::Mode::socat_tun)
    {
        m_socatTunHostDriver =
            std::make_unique<SocatTunHostDriver>(&m_posixFileIf);
        m_socatTunHostDriver->startTransfer(m_txQueue.get(), m_loop);
    }
    else if (mode == SNConfig::Mode::socat_tap)
    {
        m_addressCache = std::make_unique<AddressCache>();
        m_socatTapHostDriver = std::make_unique<SocatTapHostDriver>(
            m_addressCache.get(), &m_posixFileIf);
        m_packetTypeCodec->setAddressCache(m_addressCache.get());
        m_socatTapHostDriver->startTransfer(m_txQueue.get(), m_loop);
    }
    else if (mode == SNConfig::Mode::tap)
    {
        m_addressCache = std::make_unique<AddressCache>();
        m_tapHostDriver = m_factory->makeTapHostDriver(m_addressCache.get());
        m_tapHostDriver->setIfEventCommands(m_snConfig.m_on_if_up,
                                            m_snConfig.m_on_if_down);

        m_packetTypeCodec->setAddressCache(m_addressCache.get());
        m_tapHostDriver->startTransfer(m_txQueue.get(), m_loop);
    }

    m_packetTypeCodec->setWsDump(m_wsDump.get());
    m_packetTypeCodec->setMasterEndedCB([&]() {
        LOG_INFO << "Received master_stop. Quit client.";
        m_loop.stop();
    });

    if (m_snConfig.m_startMaster)
    {
        setupMaster();
    }
    return true;
}

void
SerialNet::setupMaster()
{
    LOG_INFO << "Starting master.";
    m_master = std::make_unique<Master>(m_ts, m_packetTypeCodec.get(),
                                        m_txQueue.get(), &m_config);

    const int masterTimeout = m_snConfig.m_masterTimeout;
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
