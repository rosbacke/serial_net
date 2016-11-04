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
 * hw_tests.cpp
 *
 *  Created on: 2 nov. 2016
 *      Author: mikaelr
 */

/**
 * Implement a number of small tests that are supposed to affect hardware.
 * Uses the Low level library classes. " different purposes:
 * - Make sure the low level hardware abstraction classes actually work in the
 * physical world.
 * - Be a test driver to debug the hardware side.
 */

#include <boost/program_options.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/options_description.hpp>

#include "utility/Log.h"
#include "utility/ProgramVersion.h"
#include <string>

#include "HwTests.h"

namespace po = boost::program_options;

void
setupOptions(po::options_description& desc)
{
    desc.add_options()                                    //
        ("usage", "Show summary of command line options") //
        ("version", "Show git version of the program.")   //
        ("cmd", po::value<std::string>(), R"(One of:
- setRTS : Set the RTS signal to 0 or 1 dep. on arg.
  )")                                                     //
        ("serial-device,d", po::value<std::string>()->default_value(""),
         "Name of serial device for byte interface.") //
        ("arg", po::value<int>()->default_value(-1),
         "cmd dependent integral argument.")                              //
        ("log-level,l", po::value<int>()->default_value(1), "Log level.") //
        ("timeout,t", po::value<int>()->default_value(-1),
         "Timeout (-1 forever).") //
        ;
}

int
main(int argc, const char* argv[])
{
    if (argc == 1)
    {
        std::cout << "\n";
        std::cout << "     'serialnet --usage' for command line options.\n\n";
        std::cout << "Version : " << ProgramVersion::getVersionString() << "\n";
        std::cout << std::endl;
        return 0;
    }

    // Declare the supported options.
    po::options_description desc("Allowed options");
    setupOptions(desc);

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("usage"))
    {
        std::cout << desc << std::endl;
        return 1;
    }
    if (vm.count("version"))
    {
        std::cout << "Program version string: "
                  << ProgramVersion::getVersionString() << std::endl;
        return 1;
    }
    Log::instance().m_level =
        static_cast<Log::Level>(vm["log-level"].as<int>());
    if (vm.count("cmd") == 0)
    {
        std::cout << "Please supply cmd argument to do something useful."
                  << std::endl;
        return 1;
    }
    auto cmd = vm["cmd"].as<std::string>();
    auto device = vm["serial-device"].as<std::string>();
    auto arg = vm["arg"].as<int>();
    auto timeout = vm["timeout"].as<int>();

    if (cmd == "setRTS")
    {
        if (device == "")
        {
            std::cout << "Need serial device.\n\n" << desc << std::endl;
            return 1;
        }
        HwTests::doSetRTS(device, arg, timeout);
    }
}
