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

#include <unistd.h>
#include "eventwrapper/EventLoop.h"

using namespace gsl;

struct Helper : public ByteEtherIf::RxIf
{
	Helper(ByteEtherIf* beIf, std::string res, 	EventLoop& loop)
		 : m_ser(beIf), m_target(res), m_loop(loop)  {}


	void receiveBytes(const gsl::span<gsl::byte>& bytes) override
	{
		for (auto i : bytes)
		{
			newByte(i);
		}
	}

	void newByte(byte myByte)
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
	EventLoop& m_loop;
};

int main(int argc, const char *argv[])
{
	EventLoop loop;

	if (argc != 2)
	{
		LOG_INFO << "\nUsage: serialtest <path to serial device>\n";
		return -1;
	}
	std::string device(argv[1]);
	SerialHalReal real;
	SerialByteEther ser(device, real.get());

	loop.onTimeout(5, [&]()->bool { std::cerr << "timeout." << std::endl; loop.stop(); return false; });
	ser.registerReadCB(&loop);

	std::string str("Hello!");
	Helper helper(&ser, str, loop);
	ser.addClient(&helper);

	auto sp = span<const byte>(reinterpret_cast<const byte *>(str.data()), str.size());
	ser.sendBytes(sp);
	loop.run();
	if (helper.m_data == helper.m_target)
	{
		return 0;
	}
	std::cerr << "Data written and read differs." << std::endl;
	return 1;
}
