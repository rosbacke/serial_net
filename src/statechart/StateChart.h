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

#include <cassert>
#include <iostream>

#include "utility/Log.h"

/**
 * Base class for all state classes. Templated on the HSM.
 * Allow customization of the HSM functions.
 * Supplies a number of helper function to be available in state context.
 */
template <class HSM>
class StateBase
{
  protected:
    StateBase(HSM& hsm) : m_hsm(&hsm)
    {
    }

    using StateId = typename HSM::StateId;

  public:
    void transition(StateId id)
    {
        m_hsm->transition(static_cast<int>(id));
    }

    HSM& hsm()
    {
        return *m_hsm;
    }

  private:
    HSM* m_hsm;
};

class FsmBaseBase;

/**
 * Base for 'StateModel' class. Classes that keep a state as a member and
 * introduce
 * inheritance for event passing.
 */
class ModelBase
{
  public:
    virtual ~ModelBase()
    {
    }
};

/**
 * Helper class. Keeps track of all state information objects for at particular
 * HSM.
 */
class FsmSetupBase
{
  public:
    struct PlacementDestroyer
    {
        void operator()(ModelBase* p)
        {
            p->~ModelBase();
        }
    };

    using UniquePtr = std::unique_ptr<ModelBase, PlacementDestroyer>;

    using CreateFkn =
        std::function<ModelBase*(char* store, FsmBaseBase* hsmbase)>;

    struct LevelData
    {
        std::vector<char> m_stateStorage;
        typename FsmSetupBase::UniquePtr m_activeState;
    };

    struct StateInfo
    {
        template <class StateId>
        StateInfo(StateId id, StateId parentId, int level, size_t stateSize,
                  const CreateFkn& fkn, const std::string& name = "")
            : m_id(static_cast<int>(id)),
              m_parentId(static_cast<int>(parentId)), m_level(level),
              m_stateSize(stateSize), m_name(name), m_maker(fkn)
        {
        }

        int m_id;
        int m_parentId;
        int m_level;
        size_t m_stateSize;
        std::string m_name;
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

    void addStateBase(int stateId, int parentId, size_t size, CreateFkn fkn);

    std::vector<StateInfo> m_states;
    int m_maxLevel = 0;
};

class FsmBaseSupport
{
  public:
    using StateInfo = FsmSetupBase::StateInfo;
    FsmBaseSupport() : m_hsm(nullptr)
    {
    }

    virtual ~FsmBaseSupport()
    {
    }

    void cleanup();

    void transition(int id);

    void setStartState(int id, FsmBaseBase* hsm);

    void prepareTransition(FsmBaseBase* fbb);

  private:
    void doTransition(const StateInfo* nextInfo);

    void populateNextInfos(const StateInfo* nextInfo);

    size_t findFirstThatDiffer();

    void doExit(size_t bl);

    void doEntry(size_t targetLevel);

  public:
    std::vector<FsmSetupBase::LevelData> m_stackFrames;
    std::vector<const StateInfo*> m_currentInfos;

    FsmSetupBase m_setup;

  private:
    std::vector<const StateInfo*> m_nextInfos;

    bool m_stateUpdate = false;
    int m_nextState = 0;
    FsmBaseBase* m_hsm = nullptr;
};

class FsmBaseBase
{
  protected:
    FsmBaseBase() : m_base()
    {
    }

    ~FsmBaseBase()
    {
        m_base.cleanup();
    }

    FsmBaseSupport m_base;

  public:
    /**
     * Call to indicate a transition to a new state after this
     * event handling call has ended. Will call suitable exit/entry
     * handlers.
     */
    void transition(int id)
    {
        m_base.transition(id);
    }
};

template <class Event>
class EventInterface : public ModelBase
{
  public:
    virtual ~EventInterface()
    {
    }
    virtual bool event(const Event& ev) = 0;
};

template <class Fsm, class St, class Event>
class StateModel : public EventInterface<Event>
{
  public:
    StateModel(FsmBaseBase* fsm) : m_state(*static_cast<Fsm*>(fsm))
    {
    }
    bool event(const Event& event) override
    {
        return m_state.event(event);
    }
    virtual ~StateModel()
    {
    }

  private:
    St m_state;
};

template <class Event>
class FsmBaseEvent : public FsmBaseBase
{
  public:
    void postEvent(const Event& ev)
    {
        // LOG_DEBUG << "Post event:" << int(ev.m_id);
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

    static bool emitEvent(ModelBase* sbb, const Event& ev)
    {
        return static_cast<EventInterface<Event>*>(sbb)->event(ev);
    }

    void processEvent(const Event& ev)
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
            eventHandled = emitEvent(activeState, ev);

        } while (!eventHandled && level > 0);
        m_base.prepareTransition(this);
    }

  private:
    VecQueue<Event> m_eventQueue;
};

/**
 * Base class based on the state description type. This level
 * is suitable for declaring a static FsmSetupBase. (To be implemented.)
 */
template <class St>
class FsmBaseSt : public FsmBaseEvent<typename St::Event>
{
};

template <class MyFsm, class St>
class FsmBase : public FsmBaseSt<St>
{
  public:
    using StateId = typename St::StateId;
    using Event = typename St::Event;

    /**
     * Set start state and perform initial jump to that state.
     * After this, it is legal to send events into the HSM.
     */
    void setStartState(StateId id)
    {
        FsmBaseBase::m_base.setStartState(static_cast<int>(id), this);
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
        auto fkn = [&](char* store, FsmBaseBase* hsm) -> ModelBase* {
            return static_cast<ModelBase*>(
                new (store) StateModel<MyFsm, State, Event>(hsm));
        };
        auto size = sizeof(StateModel<MyFsm, State, Event>);
        FsmBaseBase::m_base.m_setup.addStateBase(
            static_cast<int>(stateId), static_cast<int>(parentId), size, fkn);
    }
};

#endif /* SRC_STATECHART_STATECHART_H_ */
