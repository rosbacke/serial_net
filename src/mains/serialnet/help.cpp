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

#include "help.h"

std::string
helpMessage()
{
    std::string help = R"(
Serialnet is a small utility for setting up a network between nodes of a shared 
bus. The intended use is to create an TCP/IP subnet between nodes connected via 
e.g. an RS485 bus.

Quick start guide:
- Have 2 serial ports connected (e.g. /dev/ttyS0, /dev/ttyS1) via some
  shared medium (rs485 or a tweaked rs232 cable.)
- terminal one: serialnet --mode stdin --master --address 1 -d /dev/ttyS0
- terminal two: serialnet --mode opipe --address 2 -d /dev/ttyS1
- In terminal window for process one, type a string and press enter. It should
  appear as standard output on terminal two.
This example uses broadcast addresses. See 'usage' to add address filtering.

The protocol implemented is a token passing scheme which require one master 
node to orchestrate the message passing of the bus. Hence one node should 
activate the master functionality feature.
In its most simple form the tool provides framing of data into packets on the 
shared medium and manage access control to avoid interference between nodes.
There is no support for 'ports' as defined for e,g, tcp/ip. If several 
channels are needed, use e.g. tap and the normal tco/ip stack.

The utility has 2 main interfaces where data is passed:
- ether : Connect to the common medium where data is shared.
- host : Packet passing to the client side (possibly the host TCP/IP stack).

In addition the tool operate in one of several modes:
- none : Disable client functionality. (Master might still work)
- std_in : Standard input stream. User can write data that will sent to a 
           remote address that is set up at program start.
- std_out : Standard output stream. Wait for incoming data and write it to stdout.
            an optional filter on sender address can be set up at program start.
- std_io : Set up a bidirectional channel with another host.
- socat_tun : Connect host side to a TUN interface via the socat utility.
- socat_tap : Connect host side to a TAP interface via the socat utility.
- tap : Connect host side to a TAP interface.


Future extension: The following are ideas about future work.

Serialnet implement several drivers for the host and ether interfaces.
The ether side drivers comes in 2 categories: byte based streams and message 
based. The byte based adds a framing layer at the bottom to turn them into
a message.
Byte based streams:
- stdstream: Use stdin/stdout to read/write data. (mainly for debugging)
  Can not be used together with stdio based host interfaces.
- serial: Linux serial port. The most common use case.
- socket: Connect to a remote server socket and send bytes there.

Message based:
- mqtt: Post messages to a central mqtt server. (to be implemented)
- udp: Post messages to a given IP/UDP port. (to be implemented)

Host side interfaces are:
- tun: Connect to the Linux network stack via the TUN interface. 
  (allows tcp/ip networking over the shared bus.)
- mqtt: Fetch and post messages to an MQTT broker.
- stdstream: Use stdin/stdout to read/write data.

Not all combinations work. The following have been implemented so far:
   host interface | ether interface | mode
   stdstream        serial            stdin
   stdstream        serial            stdout

Serialnet internally defines 2 roles, client and master. The client 
send/receive user data over the common medium. It follow the token passing 
protocol in doing so. The master owns the bus and passes tokens to clients to 
allow them to send data. The master is also the main repository for the local 
address table. There must be exactly one master on the bus. The node 
implementing the master need not to be a client but can be.
)";
    return help;
}
