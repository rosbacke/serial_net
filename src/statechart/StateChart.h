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
 * StateChart.h
 *
 *  Created on: 12 nov. 2016
 *      Author: mikaelr
 */

#ifndef SRC_STATECHART_STATECHART_H_
#define SRC_STATECHART_STATECHART_H_

/**
 * The statechart module implement a hierarchical state machine. (HSM)
 * Basic idea is to use classes as states. The constructor/destructor
 * implement entry/exit functions for the states.
 * It is hierarchical so that a state can be a sub state to a base state.
 * Note, this is _not_ inheritance. The base state have a much larger
 * lifetime than the sub state.
 *
 * An HSM is created by creating a class inheriting from HsmBase<...>
 * This requires the following support types:
 * - An Event plass which is posted to the HSM.
 * - An enum class enumerating all the states.
 *
 * In the constructor of the HSM you need to call 'addState' for each
 * state that belongs to the state machine. Here you specify the the State
 * class, the state id and the id of a possible parent state.
 *
 *
 * Each state inherits from the class BaseState<Fsm> where Fsm in the type of
 * the HSM this state belongs to.
 * All events are delivered through the function 'event' that needs to be
 * overridden.
 * Internally, since only one state on a particular level is active, they share
 * allocated memory and uses placement new/delete to implement state changes.
 * There is memory for each level. Also, each state can access the base state to
 * get
 * access to stored variables there. (Does require appropriate casting.)
 *
 *
 */

#include "utility/VecQueue.h"

#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

#include <iostream>

class StateBaseBase
{
  public:
    virtual ~StateBaseBase()
    {
    }
};

/**
 * Base class for all state classes. Templated on the HSM to allow
 * passing the correct type when accessing the HSM reference from within
 * the state class.
 */
template <class HSM>
class StateBase : public StateBaseBase
{
  protected:
    StateBase(HSM& hsm) : m_hsm(&hsm)
    {
    }

    using Event = typename HSM::Event;
    using StateId = typename HSM::StateId;

  public:
    virtual bool event(const Event& ev) = 0;

    void transition(StateId id)
    {
        m_hsm->transition(id);
    }

    HSM& hsm()
    {
        return *m_hsm;
    }

    StateBase& base()
    {
        return *m_hsm->base(this);
    }

  private:
    HSM* m_hsm;
};

class FsmBaseBase
{
};

template <class MyFsm, class Ev, typename Stateid>
class FsmBase;

class FsmSetupBase
{
  public:
    struct PlacementDestroyer
    {
        void operator()(StateBaseBase* p)
        {
            p->~StateBaseBase();
        }
    };

    using UniquePtr = std::unique_ptr<StateBaseBase, PlacementDestroyer>;

    using CreateFkn =
        std::function<StateBaseBase*(char* store, FsmBaseBase* hsmbase)>;

    struct LevelData
    {
        std::vector<char> m_stateStorage;
        typename FsmSetupBase::UniquePtr m_activeState;
    };

    struct StateInfo
    {
        template <class StateId>
        StateInfo(StateId id, StateId parentId, int level, size_t stateSize,
                  const CreateFkn& fkn)
            : m_id(static_cast<int>(id)),
              m_parentId(static_cast<int>(parentId)), m_level(level),
              m_stateSize(stateSize), m_maker(fkn)
        {
        }

        int m_id;
        int m_parentId;
        int m_level;
        size_t m_stateSize;
        CreateFkn m_maker;
    };

    const StateInfo* findState(int id) const
    {
        auto i = std::find_if(m_states.begin(), m_states.end(),
                              [&](const auto& el) -> bool {
                                  return el.m_id == static_cast<int>(id);
                              });
        return i != m_states.end() ? &(*i) : nullptr;
    }

    template <class StateId>
    const StateInfo* findState(StateId id) const
    {
        return findState(static_cast<int>(id));
    }

    void addStateBase(int stateId, int parentId, size_t size, CreateFkn fkn)
    {
        int level = 0;
        if (stateId != parentId)
        {
            auto parent = findState(parentId);
            level = parent->m_level + 1;
        }
        auto t = StateInfo(stateId, parentId, level, size, fkn);
        m_states.emplace_back(t);
        if (m_maxLevel < level)
        {
            m_maxLevel = level;
        }
    }

    std::vector<StateInfo> m_states;
    int m_maxLevel = 0;
};

class FsmBaseSupport
{
  public:
    using StateInfo = FsmSetupBase::StateInfo;
    FsmBaseSupport(FsmBaseBase* hsm) : m_hsm(hsm)
    {
    }

    virtual ~FsmBaseSupport()
    {
    }

    void prepareTransition()
    {
        while (m_stateUpdate)
        {
            auto i = m_setup.findState(m_nextState);
            if (i)
            {
                m_stateUpdate = false;
                doTransition(i);
            }
        }
    }

    void doTransition(const StateInfo* nextInfo)
    {
        populateNextInfos(nextInfo);
        int bottomLevel = findFirstThatDiffer();
        doExit(bottomLevel);
        doEntry(nextInfo->m_level);
    }

    void populateNextInfos(const StateInfo* nextInfo)
    {
        auto si = nextInfo;
        auto level = si->m_level;
        m_nextInfos.resize(level + 1);
        m_nextInfos[level] = si;
        while (level > 0)
        {
            si = m_setup.findState(si->m_parentId);
            level = si->m_level;
            m_nextInfos[level] = si;
        }
    }

    size_t findFirstThatDiffer()
    {
        size_t level = 0;
        const auto srcSize = m_currentInfos.size();
        const auto dstSize = m_nextInfos.size();
        while (level < srcSize && level < dstSize &&
               m_currentInfos[level] == m_nextInfos[level])
        {
            level++;
        }
        return level;
    }

    void cleanup()
    {
        // Clean up states in the correct order.
        while (!m_stackFrames.empty())
        {
            m_stackFrames.pop_back();
        }
    }

    void doExit(size_t bl)
    {
        const auto bottomLevel = bl;
        while (m_currentInfos.size() > bottomLevel)
        {
            m_currentInfos.pop_back();
            m_stackFrames[m_currentInfos.size()].m_activeState.reset(nullptr);
        }
    }
    void doEntry(size_t targetLevel)
    {
        const auto dstLevel = m_nextInfos.size();
        while (m_currentInfos.size() != dstLevel)
        {
            auto level = m_currentInfos.size();
            auto newState = m_nextInfos[level];
            auto& frame = m_stackFrames[level];
            auto& storeVec = frame.m_stateStorage;
            if (storeVec.size() < newState->m_stateSize)
            {
                storeVec.resize(newState->m_stateSize);
            }
            frame.m_activeState.reset(
                newState->m_maker(storeVec.data(), m_hsm));
            m_currentInfos.push_back(newState);
        }
    }
    void transition(int id)
    {
        m_nextState = static_cast<int>(id);
        m_stateUpdate = true;
    }

    void setStartState(int id)
    {
        m_stackFrames.resize(m_setup.m_maxLevel + 1);
        m_nextState = static_cast<int>(id);
        m_stateUpdate = true;
        prepareTransition();
    }

    std::vector<FsmSetupBase::LevelData> m_stackFrames;
    std::vector<const StateInfo*> m_currentInfos;

    FsmSetupBase m_setup;

  private:
    std::vector<const StateInfo*> m_nextInfos;

    bool m_stateUpdate = false;
    int m_nextState = 0;
    FsmBaseBase* m_hsm = nullptr;
};

/**
 * Base class for the Fsm. Contain all the Support data and functions that
 * should be
 * accessible from the HSM.
 */
template <class MyFsm, class Ev, typename Stateid>
class FsmBase : public FsmBaseBase
{
  public:
    // using EventId = Ev;
    using Event = Ev;
    using StateId = Stateid;
    using FSetup = FsmSetupBase;
    using StateInfo = FsmSetupBase::StateInfo;

    FsmBase() : m_base(this)
    {
    }

    ~FsmBase()
    {
        m_base.cleanup();
    }

    void postEvent(const Ev& ev)
    {
        bool empty = m_eventQueue.empty();
        m_eventQueue.push(ev);
        if (empty)
        { // Nobody else is currently processing events.
            while (!m_eventQueue.empty())
            {
                // Keep a local copy in case the vector reallocate during the
                // event processing. (due to internal event posting.)
                Event ev = m_eventQueue.front();
                processEvent(ev);
                m_eventQueue.pop();
            }
        }
    }

    /**
     * Set start state and perform initial jump to that state.
     * After this, it is legal to send events into the HSM.
     */
    void setStartState(StateId id)
    {
        m_base.setStartState(static_cast<int>(id));
    }

  protected:
    /**
     * Add a root state to the HSM. Needs to be called before starting
     * the HSM. (Suggest the HSM constructor.)
     * @param State    Type name for the class that implement the state.
     *                 Must inherit StateBase<'HSM'>.
     * @param stateId  State identity number from the range given by StateId.
     */
    template <class State, StateId stateId>
    void addState()
    {
        addState<State, stateId, stateId>();
    }

    /**
     * Add a state to the HSM. Needs to be called before starting
     * the HSM. (Suggest the HSM constructor.)
     * @param State    Type name for the class that implement the state.
     *                 Must inherit StateBase<'HSM'>.
     * @param stateId  State identity number from the range given by StateId.
     * @param parentId State identity of the parent state. If == stateId,
     *                 then this is a root state.
     */
    template <class State, StateId stateId, StateId parentId>
    void addState()
    {
        auto fkn = [&](char* store, FsmBaseBase* hsm) -> StateBaseBase* {
            return static_cast<StateBase<MyFsm>*>(
                new (store) State(*static_cast<MyFsm*>(hsm)));
        };
        m_base.m_setup.addStateBase(static_cast<int>(stateId),
                                    static_cast<int>(parentId), sizeof(State),
                                    fkn);
    }

  private:
    // Allow  states to do transition.
    friend class StateBase<MyFsm>;

    /**
     * Call to indicate a transition to a new state after this
     * event handling call has ended. Will call suitable exit/entry
     * handlers.
     */
    void transition(StateId id)
    {
        m_base.transition(static_cast<int>(id));
    }

    void processEvent(const Ev& ev)
    {
        bool eventHandled = false;
        int level = m_base.m_currentInfos.size();
        if (level == 0)
        {
            return;
        }
        do
        {
            level--;
            auto activeState = m_base.m_stackFrames[level].m_activeState.get();
            eventHandled =
                static_cast<StateBase<MyFsm>*>(activeState)->event(ev);

        } while (!eventHandled && level > 0);
        m_base.prepareTransition();
    }

    FsmBaseSupport m_base;
    VecQueue<Ev> m_eventQueue;
};

#endif /* SRC_STATECHART_STATECHART_H_ */
