/*
 * serialtest.cpp
 *
 *  Created on: 30 juli 2016
 *      Author: mikaelr
 */

#include "utility/Log.h"
#include "utility/Utility.h"
#include "interfaces/ByteEtherIf.h"
#include "drivers/serial/SerialByteEther.h"

#include "hal/SerialHalReal.h"

#include <libconfig.h++>

#include "reactcpp.h"

#include <unistd.h>

using namespace gsl;

struct Helper : public ByteEtherIf::RxIf
{
	Helper(ByteEtherIf* beIf, std::string res, 	React::MainLoop& loop)
		 : m_ser(beIf), m_target(res), m_loop(loop)  {}
	virtual void newByte(byte myByte)  override
	{
		std::cerr << "Read: " << to_integer<int>(myByte) << std::endl;
		m_data.push_back(to_integer<char>(myByte));
		if (m_data == m_target)
		{
			m_loop.stop();
		}
	}
	virtual ~Helper() {}


	ByteEtherIf* m_ser;
	std::string m_data;
	std::string m_target;
	React::MainLoop& m_loop;
};

int main(int argc, const char *argv[])
{
	React::MainLoop loop;

	if (argc != 2)
	{
		LOG_INFO << "\nUsage: serialtest <path to serial device>\n";
		return -1;
	}
	std::string device(argv[1]);
	SerialHalReal real;
	SerialByteEther ser(device, real.get());

	loop.onTimeout(5, [&]()->bool { std::cerr << "timeout." << std::endl; loop.stop(); return false; });
	ser.registerReadCB(loop);

	std::string str("Hello!");
	Helper helper(static_cast<ByteEtherIf*>(&ser), str, loop);
	ser.addClient(&helper);

	for(auto i : str)
	{
		ser.sendByte(gsl::to_byte(static_cast<uint8_t>(i)));
	}
	loop.run();
	if (helper.m_data == helper.m_target)
	{
		return 0;
	}
	std::cerr << "Data written and read differs." << std::endl;
	return 1;
}
