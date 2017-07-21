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
 * SerialProtocal_test.cpp
 *
 *  Created on: 13 nov. 2016
 *      Author: mikaelr
 */

#include <gtest/gtest.h>

#include "interfaces/SerialProtocol.h"

// Test open, read, close.
TEST(SerialProtocol, Clientaddr)
{
    // Ensure the size of the headers are as expected. manual inspection.
    EXPECT_EQ(sizeof(packet::GrantToken), 2_sz);
    EXPECT_EQ(sizeof(packet::ReturnToken), 2_sz);
    EXPECT_EQ(sizeof(packet::MasterStarted), 1_sz);
    EXPECT_EQ(sizeof(packet::MasterEnded), 1_sz);
    EXPECT_EQ(sizeof(packet::SendPacket), 3_sz);
    EXPECT_EQ(sizeof(packet::MacUpdate), 10_sz);
    EXPECT_EQ(sizeof(packet::AddressDiscovery), 3_sz);
    EXPECT_EQ(sizeof(packet::AddressRequest), 13_sz);
    EXPECT_EQ(sizeof(packet::AddressReply), 10_sz);
    EXPECT_EQ(1, 1);
}
