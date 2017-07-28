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
Serialnet is a utility for setting up a network between nodes of a shared 
serial bus. The intended use is to create channel like communication between nodes 
connected via  e.g. an RS485 bus. Typical channels are TCP/IP or raw data.

Quick start: Have a look at the scripts in the tools folder. THey offer 
guidance for setting up raw streams via ptys and TCP/IP via tun/tap
interfaces.

The protocol implements a token passing scheme which require one master 
node to orchestrate the message passing of the bus. Hence exactly one node should 
activate the master functionality feature.
In its most simple form the tool provides framing of data into packets on the 
shared medium and manage access control to avoid interference between nodes.
The network has one address per node and can distinguish between a few 
data protocol (tun, tap, raw_stream). So a tcp/ip subnet carried via tap 
interfaces can coexist with several nodes doing raw_stream communication.

There is no support for retransmission, ports, or other higher level
guarantees. For that, consider using tcp/ip on tap interfaces. A common 
development test case is performing an ssh login over tap and it works well.

Overall architecture
The utility has 2 main interfaces where data is passed:
- ether : Connect to the common medium where data is shared.
- host : Packet passing to the client side (possibly the host TCP/IP stack).

Currently the only implemented ether side interface is a serial port.
In the future this can be extended to other mediums. The main requirement is that
they can carry a byte stream with occasional modest timing requirement 
between packets. It could be useful with an UDP interface for bridging serial
ports over networks. Also virtual ports for testing purposes are considered.
Also, the framing protocol is cleanly separated from the addressing, so a packet
based carrier can skip the framing.

Serial driver:
- Default setting is 115200 baud, 8N1.
- Offers a few options for hardware control:
  - RTS control : Allow using the RTS line to drive pull-up resistors in
    open-collector buses.
  - RS485 tx enable. Enable the Linux support for rs485 TX enable via RTS.
    Not tested though.
  - See separate document on issues with serial communication in modern day world.

The host side implement 3 different protocol formats:
- tap : Carry link layer frames from a TCP/IP stack.
- tun : Carry network layer frames from a TCP/IP stack.
- raw : Carry raw byte streams.

These can be used on the same network.
Each format is offered to the user in the following formats:
- tap : Via a kernel tap interface that talks directly to the tcp/ip stack.
- tun : Via a kernel tun interface that talks directly to the tcp/ip stack.
- raw : Via a pty (virtual terminal) which allow the network to simulate a 
        normal serial port.

Currently there are some legacy support for using the stdin / stdout as 
communication channels. (Useful with the socat tool).
This is in the process of being redesigned such that a single stdio driver 
can attach to any protocol driver.

Serialnet internally defines 2 roles, client and master. The client 
send/receive user data over the common medium. It follow the token passing 
protocol in doing so. The master owns the bus and passes tokens to clients to 
allow them to send data. The master is also the main repository for the local 
address table. There must be exactly one master on the bus. The node 
implementing the master need not to be a client but can be.

Adressing:
Each node has a 1 byte address. It can be statically allocated. If no address
is given to a client it tries auto allocation. Allocation:
Address   Notes
0		  Illegal address, not set etc. In some contexts, address of the master.
1         Statically allocated to the client on the same node as the master.
2-31      Reserved for statically allocated addresses.
32-63     Reserved for dynamic address allocation.
64-254    Reserved for future use.
255       Broadcast address.

Development:
- Written in standard C++14 with a few dependencies. Boost and libev.
  Mostly used Ubuntu 16.04 as development machine. See 'setup_dependencies.sh'
  script for details. Uses git/cmake for version control + build tool.
  clang-format keeps the formatting consistent.

Future directions:
- Improved testing. Need more stress testning with several clients using more bandwidth.
- Better support for starting in daemon mode at system startup.
- A low resource (C only?) client intended for microcontrollers/arduino.
  should handle address allocation so that the master dynamically adds/removes
  ptys for connected clients.
- IPv6 support. Today we use some snooping of ARP request packets that won't work in 
  IPv6. This needs to change.

)";
    return help;
}
