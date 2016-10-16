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
 * MQTTDriver.cpp
 *
 *  Created on: 29 jul 2016
 *      Author: mikaelr
 */

#include "MQTTDriver.h"

#include "mosquitto.h"
#include <memory>
#include <stdexcept>

#include "utility/Log.h"

#if 1
#define MOSQ_V_1_4_8 1
#else
#define MOSQ_V_0_15_0 1
#endif

namespace
{
// Keep an object of this alive during the use of the library.
class LibMosquittoInitShutdown
{
  public:
    LibMosquittoInitShutdown()
    {
        int res = ::mosquitto_lib_init();
        // Return value undocumented, ignore it.
        (void)res;
    }
    ~LibMosquittoInitShutdown()
    {
        int res = ::mosquitto_lib_cleanup();
        // Return value undocumented, ignore it.
        (void)res;
    }
};
}

MQTTDriverClient::MQTTDriverClient(const std::string& id) : m_connected(false)
{
#ifdef MOSQ_V_1_4_8
    m_client = ::mosquitto_new(id.c_str(), true, nullptr);
#elif defined MOSQ_0_15_0
    m_client = ::mosquitto_new(id.c_str(), nullptr);
#endif
    if (!m_client)
    {
        throw std::runtime_error("Failed to create mqtt client.");
    }
}

MQTTDriverClient::~MQTTDriverClient()
{
    ::mosquitto_destroy(m_client);
}

void
MQTTDriverClient::connect(const std::string& host)
{
    if (m_connected)
    {
        disconnect();
    }
#ifdef MOSQ_V_1_4_8
    int res = mosquitto_connect(m_client, host.c_str(), 1883,
                                60 /* seconds, keep_alive */);
#elif defined(MOSQ_V_0_15_0)
    int res = mosquitto_connect(m_client, host.c_str(), 1883,
                                60 /* seconds, keep_alive */,
                                true /* clean session */);
#endif

    if (res != MOSQ_ERR_SUCCESS)
    {
        LOG_DEBUG << "Failed connect, result:" << res;
        throw std::runtime_error("Failed connect");
    }
    m_connected = true;
}

void
MQTTDriverClient::disconnect()
{
    int res = ::mosquitto_disconnect(m_client);
    if (res != MOSQ_ERR_SUCCESS)
    {
        LOG_DEBUG << "Note: Mosq disconnected failed. Continuing.";
    }
    m_connected = false;
}

MQTTDriver::MQTTDriver()
{
    initLibrary();
}

MQTTDriver::~MQTTDriver()
{
}

std::unique_ptr<MQTTDriverClient>
MQTTDriver::getClient(const std::string& id)
{
    initLibrary();
    return std::make_unique<MQTTDriverClient>(id);
}

void
MQTTDriver::initLibrary()
{
    // Make sure the library is initialized at first boot.
    // Use Meyer static initialization.
    static LibMosquittoInitShutdown dummy;
}
