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

#include "../../src/hal/PosixFileReal.h"

#include <gtest/gtest.h>

#include "../../src/hal/PosixSleepReal.h"
#include <chrono>
#include <fstream>

int
add(int a, int b)
{
    return a + b;
}

TEST(Addition, CanAddTwoNumbers)
{
    EXPECT_TRUE(add(2, 2) == 4);
}

// Test open, read, close.
TEST(FileRead, CanReadAFile)
{
    std::fstream fstream("ReadData.txt",
                         std::fstream::out | std::fstream::trunc);
    fstream << "Test_1234";
    EXPECT_TRUE(fstream.is_open());
    fstream.close();

    PosixFileReal cutReal;
    PosixFileIf& cut = cutReal;

    int fd = cut.open("ReadData.txt", O_RDONLY);
    EXPECT_TRUE(fd > 0);

    char buf[10];
    int len = cut.read(fd, buf, 10);
    EXPECT_TRUE(len == 9);
    buf[9] = 0;
    EXPECT_TRUE(std::string("Test_1234") == std::string(buf));

    int res = cut.close(fd);
    EXPECT_TRUE(res == 0);

    // PosixFile
}

// Test open (other one), write, close.
TEST(FileWrite, CanWriteAFile)
{
    PosixFileReal cutReal;
    PosixFileIf& cut = cutReal;

    int fd = cut.open("WriteData.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    // std::cout << "fd:" << fd << " string:" << strerror(errno) << "\n";
    EXPECT_TRUE(fd > 0);

    char buf[20];
    sprintf(buf, "Test_4321");
    int len = cut.write(fd, buf, strlen(buf));

    EXPECT_TRUE(strlen(buf) == 9);
    EXPECT_TRUE(len == 9);

    int res = cut.close(fd);
    EXPECT_TRUE(res == 0);

    std::fstream fstream("WriteData.txt", std::fstream::in);
    EXPECT_TRUE(fstream.is_open());
    std::string data;
    fstream >> data;
    fstream.close();

    EXPECT_TRUE(std::string(buf) == std::string(data));

    // PosixFile
}

// Test usleep. (a bit unreliable, require that the machine is not to heavily
// loaded.
TEST(usleep, CanSleep)
{
    using namespace std::literals::chrono_literals;

    PosixSleepReal cutReal;
    PosixSleepIf& cut = cutReal;

    auto start = std::chrono::steady_clock::now();
    cut.usleep(10000);
    auto end = std::chrono::steady_clock::now();

    auto delta = end - start;

    // std::cout << "Delta time:" <<
    // std::chrono::duration_cast<std::chrono::microseconds>(delta).count();
    EXPECT_TRUE(delta > 10000us);
    EXPECT_TRUE(delta < 30000us);
}

int
main(int ac, char* av[])
{
    testing::InitGoogleTest(&ac, av);
    return RUN_ALL_TESTS();
}
