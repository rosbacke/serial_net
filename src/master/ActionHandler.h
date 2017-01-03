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
 * Addresses.h
 *
 *  Created on: 20 okt. 2016
 *      Author: mikaelr
 */

#ifndef SRC_MASTER_ACTIONHANDLER_H_
#define SRC_MASTER_ACTIONHANDLER_H_

#include "Action.h"
#include "AddressLine.h"
#include "DynamicHandler.h"
#include "interfaces/SerialProtocol.h"
#include "utility/Config.h"

#include "MasterScheduler.h"
#include <queue>
#include <vector>

/**
 * A table with all the active addresses that the master cares about.
 * Handle static address statuses.
 */
class ActionHandler
{
  public:
    /**
     * Set up AddressTable.
     */
    ActionHandler(EventLoop& loop, Config* cfg, MasterScheduler& ms,
                  DynamicHandler* dh);
    ~ActionHandler(){};

    void gotReturnToken();

    void tokenTimeout();

    // Report that the last sent out token resulted in a packet transmission.
    void packetStarted();

    void addressQueryDone();

    // Query the next token action to perform.
    Action nextAction();

    void addDynamic(LocalAddress local);

  private:
    void updateAddressLine(AddressLine::State newState);

    void updateReadyQueue();

    double nextTime(AddressLine::State state);

    AddressLine* find(LocalAddress address);
    void removeLine(AddressLine* line);

    std::vector<AddressLine> m_table;

    bool m_frontInProgress = false;

    int m_minAddr;
    int m_maxAddr;
    EventLoop& m_loop;

    Config* m_config;
    MasterScheduler& m_scheduler;
    DynamicHandler* m_dynamic;
};

std::string
toString(Action::Cmd state);

#endif /* SRC_MASTER_ACTIONHANDLER_H_ */
