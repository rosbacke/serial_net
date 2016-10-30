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
 * Master.h
 *
 *  Created on: 6 jul 2016
 *      Author: mikaelr
 */

#ifndef SRC_CORE_MASTER_H_
#define SRC_CORE_MASTER_H_

#include "Addresses.h"
#include "core/Core.h"
#include "core/PacketTypeCodec.h"

#include <reactcpp.h>

class Config;

/**
 * Implement the master part of the serial shared protocol.
 */
class Master : MasterRxIf::RxIf
{
  public:
    Master(React::Loop& loop, MasterRxIf* mr, MasterTxIf* mt,
           int ownClientAddress, Config* cfg);

    virtual ~Master();

    // void packetReceived(const std::vector<uint8_t>& data);

    // General 'doProcessing' call to be called whenever something
    // needs to be done.
    void execute();

    void exitMaster();

    class Event
    {
      public:
        enum class Id
        {
            entry,
            exit,
            init,
            rx_grant_token,
            rx_client_packet,
            rx_return_token,
            token_timeout
        };
        static std::string toString(Id id);
    };

    using EvId = Event::Id;

    typedef bool (*StateFkn)(Master& self, const EvId& ev);

    static std::string toString(StateFkn fkn);

  private:
    // Helper function to set up a timeout, including protection against
    // callbacks after master destruction.
    void makeTimeout(double time);

    void makeTimeoutAbs(double timeout);

    void cleanTimeout();

    struct State
    {
        Master::StateFkn m_fkn;
        const char* m_str;
    };

    static std::vector<State> m_states;

    // Startup state.
    static bool initState(Master& me, const EvId& ev);

    // Master has the token. No client is allowed on the line.
    static bool idleState(Master& me, const EvId& ev);

    // transition state. Try to send the token to the next.
    static bool sendNextTokenState(Master& me, const EvId& ev);

    // Token is being transmitted to client.
    static bool sendingState(Master& me, const EvId& ev);

    // Token is sent. Waiting for client to start sending.
    static bool sentState(Master& me, const EvId& ev);

    // Client is sending a packet.
    static bool receivingState(Master& me, const EvId& ev);

    void transition(StateFkn fkn)
    {
        m_nextState = fkn;
    }

    void stateProcess(const EvId& ev);
    void tokenTimeout();

    // Implement reception of packets.
    virtual void
    masterPacketReceived(MessageType type,
                         const MsgEtherIf::EtherPkt& packet) override;

    // Send the token to the next client.
    void sendToken(int destAddr);
    void sendMasterStartStop(bool stop);

    void postEvent(EvId event);

    void emitEvent(const EvId& ev);

    React::Loop& m_loop;

    MasterRxIf* m_masterRx;
    MasterTxIf* m_masterTx;

    std::shared_ptr<React::TimeoutWatcher> m_tokenTimeout;

    int m_nextToken;

    StateFkn m_state;
    StateFkn m_nextState;

    int m_ownClientAddress;
    std::deque<EvId> m_events;

    // Helper for delayed callback. Will derive a weak ptr to this
    // that is handed out to e.g. external timers etc.
    // If *this is deleted when callback happens, weak_ptr return nullptr.
    std::shared_ptr<void*> m_timeoutHelper;

    Config* m_config;

    bool m_anyPacketSent = false;
    Addresses m_addresses;
};

#endif /* SRC_CORE_MASTER_H_ */
