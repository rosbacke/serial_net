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
 * StateChart.cpp
 *
 *  Created on: 12 nov. 2016
 *      Author: mikaelr
 */

#include "StateChart.h"

void
FsmSetupBase::addStateBase(int stateId, int parentId, size_t size,
                           CreateFkn fkn, std::string name)
{
    int level = 0;
    if (stateId != parentId)
    {
        auto parent = findState(parentId);
        level = parent->m_level + 1;
    }
    auto t = StateInfo(stateId, parentId, level, size, fkn, name);
    m_states.emplace_back(t);
    if (m_maxLevel < level)
    {
        m_maxLevel = level;
    }
}

void
FsmBaseSupport::prepareTransition(FsmBaseBase* fbb)
{
    m_hsm = fbb;
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

void
FsmBaseSupport::doTransition(const StateInfo* nextInfo)
{
    assert(nextInfo != nullptr);
    populateNextInfos(nextInfo);
    assert(!m_nextInfos.empty());
    int bottomLevel = 0;
    if (!m_currentInfos.empty())
    {
        bottomLevel = findFirstThatDiffer();
    }
    doExit(bottomLevel);
    doEntry(nextInfo->m_level);
}

void
FsmBaseSupport::populateNextInfos(const StateInfo* nextInfo)
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

size_t
FsmBaseSupport::findFirstThatDiffer()
{
    // Handle the case for transition to self.
    if (m_currentInfos.back() == m_nextInfos.back())
    {
        // Do exit/entry.
        return m_currentInfos.back()->m_level;
    }
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

void
FsmBaseSupport::cleanup()
{
    doExit(0);
    // Clean up states in the correct order.
    while (!m_stackFrames.empty())
    {
        m_stackFrames.pop_back();
    }
}

void
FsmBaseSupport::doExit(size_t bl)
{
    const auto bottomLevel = bl;
    while (m_currentInfos.size() > bottomLevel)
    {
        m_stackFrames[m_currentInfos.size() - 1].m_activeState.reset(nullptr);
        m_currentInfos.pop_back();
    }
}
void
FsmBaseSupport::doEntry(size_t targetLevel)
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

        m_currentInfos.push_back(newState);
        frame.m_activeState.reset(newState->m_maker(storeVec.data(), m_hsm));
    }
}
void
FsmBaseSupport::transition(int id)
{
    m_nextState = id;
    m_stateUpdate = true;
}

void
FsmBaseSupport::setStartState(int id, FsmBaseBase* hsm)
{
    m_stackFrames.resize(m_setup.m_maxLevel + 1);
    m_nextState = static_cast<int>(id);
    m_stateUpdate = true;
    prepareTransition(hsm);
}
