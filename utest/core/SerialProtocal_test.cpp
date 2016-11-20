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
