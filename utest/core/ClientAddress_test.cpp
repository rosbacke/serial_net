/*
 * PosixFileIfReal_test.cpp
 *
 *  Created on: 30 okt. 2016
 *      Author: mikaelr
 */

#include <gtest/gtest.h>

#include <fstream>
#include <chrono>

#include "core/ClientAddress.h"

// Test open, read, close.
TEST(ClientAddress, Clientaddr)
{
	EXPECT_EQ(1, 1);
	ClientAddress ac(nullptr);
	ac.setupUniqueId();
}

int main(int ac, char* av[])
{
  testing::InitGoogleTest(&ac, av);
  return RUN_ALL_TESTS();
}
