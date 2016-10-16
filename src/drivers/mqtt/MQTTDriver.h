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
 * MQTTDriver.h
 *
 *  Created on: 29 jul 2016
 *      Author: mikaelr
 */

#ifndef SRC_DRIVERS_MQTTDRIVER_H_
#define SRC_DRIVERS_MQTTDRIVER_H_

#include <memory>
#include <string>
#include <utility>

struct mosquitto;

class MQTTDriverClient
{
  public:
    MQTTDriverClient(const std::string& id);
    ~MQTTDriverClient();

    void connect(const std::string& host = "127.0.0.1");
    void disconnect();

  private:
    struct mosquitto* m_client; // Must be valid.
    bool m_connected;
};

/**
 * Implement an MQTT driver. Allow for:
 * - using the broker as a common medium, both on byte and msg level.
 * - using the broker as an interface for local packet sending.
 */
class MQTTDriver
{
  public:
    MQTTDriver();
    ~MQTTDriver();

    static std::unique_ptr<MQTTDriverClient> getClient(const std::string& id);

  private:
    static void initLibrary();
};

#endif /* SRC_DRIVERS_MQTTDRIVER_H_ */
