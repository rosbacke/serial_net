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
 * MasterFSM.h
 *
 *  Created on: 13 nov. 2016
 *      Author: mikaelr
 */

#ifndef SRC_MASTER_MASTERFSM_H_
#define SRC_MASTER_MASTERFSM_H_

#include "../eventwrapper/EventLoop.h"
#include "Event.h"
#include "MasterUtils.h"
#include "utility/VecQueue.h"

#include "statechart/StateChart.h"

class Master;
class Config;

class States
{
  public:
    using Event = ::Event;
    enum class StateId
    {
        init,
        idle,
        sendingToken,
        waitClientPacketDone,
        queryAddress,
        waitAddressReply
    };
};

class MasterHSM : public FsmBase<MasterHSM, States>
{
  public:
    MasterHSM(Master* master, Config* config, EventLoop& loop);

    Master* m_master = nullptr;
    Config* m_config = nullptr;
    Timer m_timer;
};

#endif /* SRC_MASTER_MASTERFSM_H_ */
