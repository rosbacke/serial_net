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
#include "interfaces/TimeServiceIf.h"
#include "utility/Config.h"
#include "utility/Timer.h"

#include "MasterScheduler.h"

#include <functional>
#include <queue>
#include <vector>

class TimeServiceIf;

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
    ActionHandler(TimeServiceIf& ts, Config* cfg,
                  std::function<void(const Action& a)> triggerAction);
    ~ActionHandler(){};

    void setDynamic(DynamicHandler* dh)
    {
        m_dynamic = dh;
    }
    // Query the next token action to perform. Main driver for events to be
    // done.
    // Action nextAction();

    void reportActionResult(Action::ReturnValue rv);

    void postActionNow(Action a)
    {
        a.m_reportCB = [&](Action::ReturnValue rv) {
            this->reportActionResult(rv);
        };
        m_scheduler.addActionNow(a);
    }

    void postAction(Action a, double time)
    {
        a.m_reportCB = [&](Action::ReturnValue rv) {
            this->reportActionResult(rv);
        };
        m_scheduler.addAction(a, time);
    }

    void checkNewAction();

  private:
    TimeServiceIf& m_ts;

    Config* m_config;
    MasterScheduler m_scheduler;
    DynamicHandler* m_dynamic = nullptr;
    std::function<void(const Action& a)> m_triggerNewAction = nullptr;
    TimeServiceIf::Timer m_delayAction;
    Action m_currentAction;
};

#endif /* SRC_MASTER_ACTIONHANDLER_H_ */
