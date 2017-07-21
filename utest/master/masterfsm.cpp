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
 * PosixFileIfReal_test.cpp
 *
 *  Created on: 30 okt. 2016
 *      Author: mikaelr
 */

#include "eventwrapper/EventLoop.h"
#include "hal/TimeServiceEv.h"
#include "interfaces/MasterTxIf.h"
#include "master/MasterFSM.h"
#include "master/MasterPacketTx.h"
#include <gtest/gtest.h>
#include <vector>

class MasterTxMock : public MasterTxIf
{
  public:
    MasterTxMock() : m_sent()
    {
    }

    void sendMasterPacket(const MsgEtherIf::EtherPkt& packet)
    {
        m_sent.clear();
        for (auto i : packet)
            m_sent.push_back(i);
    }

    // Return true if there are no packets queued for transmission.
    bool txQueueEmpty() const
    {
        return false;
    }

    // When the own client can send a packet.
    void sendClientPacket()
    {
    }

    // Return the currently used client address.
    LocalAddress clientAddress() const
    {
        return LocalAddress::null_addr;
    }

    bool empty() const
    {
        return m_sent.empty();
    }
    // Return true if the last packet sent was a 'master started' packet.
    bool isMasterStartedSent() const
    {
        if (m_sent.size() != sizeof(packet::MasterStarted))
            return false;
        auto p = packet::toHeader<packet::MasterStarted>(m_sent.data());
        if (p->m_type != MessageType::master_started)
            return false;
        return true;
    }

    std::vector<gsl::byte> m_sent;
};

TEST(MasterFSM, testconstruction)
{
    EventLoop loop;
    TimeServiceEv ev(loop);

    MasterFSM fsm(ev, nullptr, nullptr, nullptr, nullptr);
    fsm.setStartState(States::StateId::idle);

    EXPECT_FALSE(fsm.actionActive());
    EXPECT_EQ(fsm.currentAction().m_action, Action::Cmd::do_nothing);
}

class TestDriver
{
  public:
    TestDriver()
        : m_ev(loop), mpTx(&masterMock), fsm(m_ev, nullptr, &mpTx, nullptr, nullptr)
    {
        fsm.setStartState(States::StateId::idle);
        EXPECT_TRUE(masterMock.empty());
    }

    void start_test(Action a)
    {
        EXPECT_TRUE(masterMock.empty());

        a.m_reportCB = [&](Action::ReturnValue rv) {
            result_callback_called = true;
            return_value = rv;
        };
        fsm.startAction(a);

        EXPECT_TRUE(fsm.actionActive());
        EXPECT_EQ(fsm.currentAction().m_action, a.m_action);
        EXPECT_TRUE(masterMock.isMasterStartedSent());
    }

    void end_test(Action::ReturnValue expectedValue)
    {
        loop.run();
        // Exit loop when there are no more actions to be taken.

        EXPECT_EQ(return_value, expectedValue);
        EXPECT_TRUE(result_callback_called);

        EXPECT_FALSE(fsm.actionActive());
        EXPECT_EQ(fsm.currentAction().m_action, Action::Cmd::do_nothing);
    }

    EventLoop loop;
    TimeServiceEv m_ev;
    MasterTxMock masterMock;
    MasterPacketTx mpTx;
    MasterFSM fsm;

    Action::ReturnValue return_value = Action::ReturnValue::not_set;
    bool result_callback_called = false;
};

TEST(MasterFSM, test_master_start_action_with_ok)
{
    TestDriver td;

    auto action = Action::makeMasterStartAction();
    td.start_test(action);

    // We sent out a master started. Normally this is seen on the input side
    // and reported back to the fsm.
    td.fsm.postEvent(Event::Id::rx_pkt_master_start_stop);

    td.end_test(Action::ReturnValue::ok);
}

TEST(MasterFSM, test_master_start_action_with_timeout)
{
    TestDriver td;

    auto action = Action::makeMasterStartAction();
    td.start_test(action);

    // We sent out a master started. Normally this is seen on the input side
    // and reported back to the fsm. However, skipping it here.
    // td.fsm.postEvent(Event::Id::rx_pkt_master_start_stop);

    td.end_test(Action::ReturnValue::timeout);
}

int
main(int ac, char* av[])
{
    testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}
