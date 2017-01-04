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

#include "hal/PosixFileReal.h"
#include "statechart/StateChart.h"

#include <gtest/gtest.h>


enum class TestEventId
{
	testEvent1,
	testEvent2,
	testEvent3,
};

class TestEvent
{
public:
	TestEvent(TestEventId id) : m_id(id) {}
	TestEventId m_id;
};

static int testData = -1;


class TestStates
{
public:
	enum class StateId
	{
		state1,
		state2,
		state3,
	};

	using Event = TestEvent;


};


class MyTestFsm : public FsmBase<MyTestFsm, TestStates>
{

public:
	MyTestFsm();

	int testD2 = -2;
};

using std::cout;
using std::endl;

class TestState : public StateBase<MyTestFsm>
{
public:
	TestState(MyTestFsm& fsm) : StateBase(fsm)
	{
		testData = 0;
		cout << "State1, entry" << endl;
	}

	~TestState()
	{
		testData = 10;
		cout << "State1, exit" << endl;
	}

	bool event(const TestEvent& ev)
	{
		cout << "State1, event : " << int(ev.m_id) << endl;
		testData = 1;
		if (ev.m_id == TestEventId::testEvent1)
		{
			transition(StateId::state2);
		}
		if (ev.m_id == TestEventId::testEvent3)
		{
			transition(StateId::state3);
		}
		return false;
	}
};

class TestState2 : public StateBase<MyTestFsm>
{
public:
	TestState2(MyTestFsm& fsm) : StateBase(fsm)
	{
		testData = 5;
		cout << "State2, entry" << endl;
	}

	~TestState2()
	{
		testData = 11;
		cout << "State2, exit" << endl;
	}

	bool event(const TestEvent& ev)
	{
		cout << "State2, event : " << int(ev.m_id) << endl;

		if (ev.m_id == TestEventId::testEvent1)
		{
			transition(StateId::state1);
			testData = 8;
		}
		if (ev.m_id == TestEventId::testEvent2)
		{
			testData = 15;
			hsm().testD2 = 2;
			return false;
		}
		testData = 9;
		return false;
	}
};

MyTestFsm::MyTestFsm()
{
	addState<TestState, StateId::state1>();
	addState<TestState2, StateId::state2>();
	addState<TestState2, StateId::state3, StateId::state1>();
}

TEST(StateChart, testStateChart)
{
	using StateId = TestStates::StateId;
	cout << "start" << endl;
	MyTestFsm myFsm;
	cout << "do 2" << endl;

	EXPECT_EQ(-2, myFsm.testD2);

	TestEvent ev2(TestEventId::testEvent2);
	TestEvent ev1(TestEventId::testEvent1);
	TestEvent ev3(TestEventId::testEvent3);

	cout << "do 3" << endl;
	EXPECT_EQ(-1, testData);
	myFsm.setStartState(StateId::state1);
	EXPECT_EQ(0, testData);
	cout << "do 4" << endl;

	myFsm.postEvent(ev2);
	EXPECT_EQ(1, testData);
	cout << "do 5" << endl;

	myFsm.postEvent(ev1); // Pass over to state2.
	EXPECT_EQ(5, testData);
	EXPECT_EQ(-2, myFsm.testD2);
	cout << "do 6" << endl;

	myFsm.postEvent(ev2);
	EXPECT_EQ(15, testData);
	EXPECT_EQ(2, myFsm.testD2);
	cout << "do 7" << endl;

	myFsm.postEvent(ev1);
	EXPECT_EQ(0, testData);
	EXPECT_EQ(2, myFsm.testD2);
	cout << "do 8" << endl;

	myFsm.postEvent(ev3); // Pass over to state3.
}

int main(int ac, char* av[])
{
  testing::InitGoogleTest(&ac, av);
  return RUN_ALL_TESTS();
}
