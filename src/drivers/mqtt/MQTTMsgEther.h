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
 * MQTTMsgEther.h
 *
 *  Created on: 30 jul 2016
 *      Author: mikaelr
 */

#ifndef SRC_DRIVERS_MQTTMSGETHER_H_
#define SRC_DRIVERS_MQTTMSGETHER_H_

#include "interfaces/MsgEtherIf.h"
#include <memory>

class MQTTDriverClient;

/**
 * Interface for sending full packets over the MQTT broker.
 */
class MQTTMsgEther : public MsgEtherIf
{
  public:
    MQTTMsgEther();
    ~MQTTMsgEther();

    // Set up a connection to a broker.
    void connect(const std::string& ip, const std::string& topicRx,
                 const std::string& topicTx);

    /// Implement interface.
    // Send a message to the ether.
    virtual void sendMsg(const ByteVec& msg) override;

    // Register a receiver.
    virtual void addClient(MsgEtherIf::RxIf* cb) override;

  private:
    std::unique_ptr<MQTTDriverClient> m_driver;
};

#endif /* SRC_DRIVERS_MQTTMSGETHER_H_ */
