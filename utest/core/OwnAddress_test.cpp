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

#include <gtest/gtest.h>

#include <chrono>
#include <fstream>

#include "core/OwnAddress.h"

TEST(OwnAddress, test_that_it_can_be_constructed_with_null_addr)
{
    OwnAddress oa;
    EXPECT_EQ(oa.addr(), LocalAddress::null_addr);
}

TEST(OwnAddress, test_that_it_can_be_constructed_with_valid_addr)
{
    OwnAddress oa(toLocalAddress(1));
    EXPECT_EQ(oa.addr(), toLocalAddress(1));
    EXPECT_TRUE(oa.valid());
}

TEST(OwnAddress, test_that_we_can_check_valid_address)
{
    OwnAddress oa;
    EXPECT_FALSE(oa.valid());

    oa.set(toLocalAddress(1));
    EXPECT_TRUE(oa.valid());
}

namespace
{
class FakeCB : public MsgHostIf::AddrChange
{
  public:
    void msgHostRx_newAddr(LocalAddress la) override
    {
        m_la = la;
    }
    LocalAddress m_la = LocalAddress::null_addr;
};
}

TEST(OwnAddress, test_that_we_ca_set_a_callback)
{
    OwnAddress oa;
    FakeCB cb;

    oa.setListener(&cb);
    EXPECT_EQ(cb.m_la, LocalAddress::null_addr);
    EXPECT_FALSE(oa.valid());

    oa.set(toLocalAddress(1));
    EXPECT_EQ(cb.m_la, toLocalAddress(1));
    EXPECT_TRUE(oa.valid());
}
