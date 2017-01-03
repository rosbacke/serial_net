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

#include "../eventwrapper/EventLoop.h"

class Config;
class MasterScheduler;
class MasterPacketTx;
class ActionHandler;

/**
 * Responsible for implementing the logic regarding Dynamic address
 * allocation.
 */
class DynamicHandler
{
  public:
    DynamicHandler(EventLoop& loop, Config* cfg, MasterScheduler* ms,
                   MasterPacketTx* tx);
    ~DynamicHandler();

    void setActionHandler(ActionHandler* ah)
    {
        m_ah = ah;
    }

    void receivedAddressRequest(const packet::AddressRequest& aReq);

    using UniqueId = packet::UniqueId;
    struct DynamicAddr
    {
        DynamicAddr(UniqueId uniqueId, LocalAddress local)
            : m_uniqueId(uniqueId), m_local(local)
        {
        }

        UniqueId m_uniqueId;
        LocalAddress m_local;
    };

    void releaseAddress(LocalAddress local);

  private:
    LocalAddress allocAddress(UniqueId id);

    EventLoop& m_loop;

    // Set of currently free addresses.
    std::vector<LocalAddress> m_freeAddresses;

    DynamicAddr* find(LocalAddress local);
    DynamicAddr* find(UniqueId id);

    // Set of currently active dynamically allocated
    std::vector<DynamicAddr> m_allocAddresses;

    MasterScheduler* m_scheduler = nullptr;
    MasterPacketTx* m_tx = nullptr;
    ActionHandler* m_ah = nullptr;
};

#endif /* SRC_MASTER_DYNAMICHANDLER_H_ */
