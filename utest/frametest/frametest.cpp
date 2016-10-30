/*
 * frametest.cpp
 *
 *  Created on: 14 jul 2016
 *      Author: mikaelr
 */

#include <cassert>
#include <iostream>

#include "ByteEther.h"
#include "core/FrameClient.h"
#include "utility/Log.h"

#if 0
/**
 * Test that we can send framed packets over the distributed serial
 * network.
 */
static int frametest1()
{
	ByteEther byteEther;

	FrameClient fc1(&byteEther);
	FrameClient fc2(&byteEther);
	FrameClient fc3(&byteEther);

	ByteBuf bb(std::string("Hello!"));
	fc1.send(bb);

	while(byteEther.execute())
		;
	assert(!fc1.empty());
	assert(!fc2.empty());
	assert(!fc3.empty());

	std::string fc1str = fc1.popRx().str();
	std::cout << "fc1 : " << fc1str << std::endl;
	assert(fc1str == "Hello!");

	std::string fc2str = fc2.popRx().str();
	std::cout << "fc2 : " << fc2str << std::endl;
	assert(fc2str == "Hello!");

	std::string fc3str = fc3.popRx().str();
	std::cout << "fc3 : " << fc3str << std::endl;
	assert(fc3str == "Hello!");

	assert(fc1.empty());
	assert(fc2.empty());
	assert(fc3.empty());
	return 0;
}


/**
 * Test that we can send addressed packets to their destination
 * over the serial net
 */
static int frametest2()
{
	ByteEther byteEther;

	FrameClient fc1(&byteEther, 1);
	FrameClient fc2(&byteEther, 2);
	FrameClient fc3(&byteEther, 3);

	ByteBuf bb(std::string("Hello!"));
	fc1.send(bb, 2);

	while(byteEther.execute())
		;

	// Make sure the client with address 2 receives the packet.
	assert(fc1.empty());
	assert(!fc2.empty());
	assert(fc3.empty());

	std::string fc2str = fc2.popRx().str();
	LOG() << "fc2 : " << fc2str;
	assert(fc2str == "Hello!");

	assert(fc2.empty());

	return 0;
}

int main()
{

	LOG() << "Hello!";

	frametest1();
	frametest2();
	return 0;
}
#endif

int main()
{
}
