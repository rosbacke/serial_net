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
 * main.cpp
 *
 *  Created on: 31 juli 2016
 *      Author: mikaelr
 */

#include <boost/program_options.hpp>
#include <boost/program_options/option.hpp>
#include <boost/program_options/options_description.hpp>

#include "SerialNet.h"
#include "help.h"

#include "utility/ProgramVersion.h"

#include "utility/Log.h"

namespace po = boost::program_options;

void
setupOptions(po::options_description& desc)
{
    desc.add_options()("help", "produce help message")(
        "usage", "Show summary of command line options")(
        "version", "Show git version of the program.")(
        "serial-device,d", po::value<std::string>(),
        "Name of serial device for byte interface.")(
        "mode", po::value<std::string>(),
        "Mode the program should work in. Allowed: std_in, std_out, std_io, "
        "socat_tun, socat_tap, tap.")(
        "address", po::value<int>()->default_value(255),
        "Local address on the serial net. Value between 1 - 32")(
        "dest_address", po::value<int>()->default_value(255),
        "For mode ipipe. Address where to send incoming data to. Value between "
        "1 - 32")("master,m",
                  "Start the master part. Exactly one master "
                  "should be active for each serial_net.")(
        "mtimeout", po::value<int>()->default_value(-1),
        "Stop the master after given amount of time (sec)")(
        "log,l", po::value<int>()->default_value(2),
        "Log level. (0=trace, 4=error)")(
        "wsdump", po::value<std::string>(),
        "Dump all serial packets to a named pipe, suitable for test2pcap")(
        "endwithmaster",
        "Quit the client if we receive a 'master_stop' message.");
}

int
main(int argc, const char* argv[])
{
    if (argc == 1)
    {
        std::cout << "\n";
        std::cout << "Use: 'serialnet --help' for overview of the tool.\n";
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

    if (vm.count("help"))
    {
        std::cout << helpMessage() << "\n";
        return 1;
    }
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
    Log::instance().m_level = static_cast<Log::Level>(vm["log"].as<int>());
    if (vm.count("mode") == 0)
    {
        std::cout << "Please supply mode argument to do something useful."
                  << std::endl;
        return 1;
    }
    if (vm.count("serial-device") < 1)
    {
        std::cout << "Need serial device.\n\n" << desc << std::endl;
        return 1;
    }

    SerialNet sn;
    sn.start(vm);
    sn.mainLoop();
    return 0;
}
