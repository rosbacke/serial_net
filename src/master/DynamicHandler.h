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
 * DynamicHandler.h
 *
 *  Created on: 25 nov. 2016
 *      Author: mikaelr
 */

#ifndef SRC_MASTER_DYNAMICHANDLER_H_
#define SRC_MASTER_DYNAMICHANDLER_H_

#include "interfaces/SerialProtocol.h"
#include <vector>

#include "AddressLine.h"
#include "eventwrapper/EventLoop.h"
#include "master/DynamicAddress.h"
#include "utility/Timer.h"

#include "interfaces/TimeServiceIf.h"

class Config;
class MasterScheduler;
class MasterPacketTx;
class ActionHandler;
class AddressLine;
class TimeServiceIf;


/**
 * Responsible for implementing the logic regarding Dynamic address
 * allocation.
 */
class DynamicHandler
{
  public:
    using UniqueId = packet::UniqueId;

    DynamicHandler(TimeServiceIf& ts, Config* cfg, MasterPacketTx* tx);
    ~DynamicHandler();

    void start();

    void setAH(ActionHandler* ah)
    {
        m_ah = ah;
    }

    // Report back that the last address discovery has finished.
    void addressDiscoveryIsDone();

    void receivedAddressRequest(const packet::AddressRequest& aReq);

    void updateAddressLine(LocalAddress addr, AddressLine::State newState);

    double nextTime(AddressLine::State state);

  private:
    LocalAddress allocAddress(UniqueId id);

    AddressLine* findLine(UniqueId id);
    AddressLine* findLine(LocalAddress address);

    void removeLine(AddressLine* line);

    int m_minAddr;
    int m_maxAddr;

    TimeServiceIf& m_ts;
    // EventLoop& m_loop;

    // Set of currently free addresses.
    std::vector<LocalAddress> m_freeAddresses;

    std::vector<AddressLine> m_table;

    MasterPacketTx* m_tx = nullptr;
    ActionHandler* m_ah = nullptr;


    TimeServiceIf::Timer m_timer;
    //Timer m_pollTimer;
    Config* m_config = nullptr;
};

#endif /* SRC_MASTER_DYNAMICHANDLER_H_ */
