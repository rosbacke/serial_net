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

#include "MasterFSM.h"
#include "MasterPacketTx.h"
#include "MasterScheduler.h"
#include "MasterUtils.h"

#include "core/PacketTypeCodec.h"

#include "ActionHandler.h"
#include "DynamicHandler.h"

#include "Event.h"

class Config;

/**
 * Implement the master part of the serial shared protocol.
 */
class Master : MasterPacketIf::RxIf
{
  public:
    Master(EventLoop& loop, MasterPacketIf* mr, MasterTxIf* mt, Config* cfg);

    ~Master();

    void exitMaster();

    using EvId = Event::Id;

    // Implement reception of packets.
    virtual void
    masterPacketReceived(MessageType type,
                         const MsgEtherIf::EtherPkt& packet) override;

    EventLoop& m_loop;
    MasterPacketIf* m_masterRx;

    Config* m_config;
    MasterPacketTx m_tx;
    MasterScheduler m_scheduler;
    DynamicHandler m_dynamicHandler;
    ActionHandler m_actionHandler;
    MasterHSM m_fsm;
};

#endif /* SRC_CORE_MASTER_H_ */
