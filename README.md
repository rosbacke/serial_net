SerialNet
=========

SerialNet is a project to allow TCP/IP communication over a common serial bus.
Main purpose is to allow using computers/microcontrollers with a simple UART
interface to be connected to a common signal wire and then allow them to use
modern TCP/IP software stacks to communicate. Current code runs on Linux
and is written in fairly portable C++14. (Non portable parts mostly in
serial port drivers and TCP/IP interfaces.)

ABOUT
=====

The SerialNet started as a private side project by me (Mikael Rosbacke)
to see if something useful would emerge when combining old school serial
ports, the bus topology of e.g. 10Base-T ethernet and modern embedded
controller such as RaspberryPI and Cortex-M series microcontrollers.

Turns out it is possible to get something useful. You can connect 2 or more
Linux computers to a serial bus and perform e.g. ssh login between them, while
running 115200 baud on the link.

Intended usage is as a link between cost constrained devices where you still
need a wire for power supply. With some clever HW hacking both power and
signalling can go over a common pair. For more range and speed, RS485 with a
pair of power cables could be suitable.

See separate INSTALL file for instructions on compiling, setup.
See separate serial_hardware.txt file for a discussion on various cheap
hardware solutions for constructing the bus.
This is highly EXPERIMENTAL software still in alpha state. Use it at your
own risk. No responsibility can be assumed.

Licencing
=========

The software is copyrighted by Mikael Rosbacke. The public releases are made
available under the GPL v3 license. This might change in the future depending
upon usefulness and who uses this software. If you need other licensing, get
in touch to discuss options.
2 third party dependecies are included. They are copyrighted by their original
authors and distributed under a separate licence:
 * GSL : Guidelines support library, licenced from Microsoft under an MIT
   license
 * ReactCPP : Released by CopernicaMarketingSoftware, Licenced under the
   Apache licence v.2

High level overview
===================

The SerialNet utility tries to solve 2 primary issues:
- Segmentation of a byte stream into packets.
- Arbitration of access to the common bus.

Other than that, there is support software to connect to serial ports on
the Linux system and pass packet to/from a Linux TAP interface.
By passing the ethernet frames over the serial bus in an orderly manner
a full TCP/IP connectivity can be achieved, including ARP request handling.

The requirement on the medium from a SW standpoint is that it is a
bytestream and occacionally the timing between bytes matters
(due to timeouts.). These should normally be rare and most packet flow
follow a logical order. No serial 'break' signals are used. This should
allow for a good amount of design freedom on the hardware side.

One unit on the bus is designated the 'bus master'. It owns the right to
send packets to the bus by default. It polls the other clients by passing
a token. Upon receiving the token, the client can choose to send a packet
or return the token to the master.

Each member of the bus has a local 1 byte address. The Master has address 0
and address 255 is a broadcast adress. This and clever token query patterns
allow the overhead of the token passing to be manageable.

Currently the only available addressing mode is static local address
assignment. Future plans include dynamic local address assignment
when a unit is connected to the bus. During the design a limit of 32 units
on the bus has been bart of the design, though the only hard limit is currently
the local address byte.

Currently it uses the utility 'socat' to get access to the TUN/TAP devices.
Plan is to incorporate this support into SerialNet once it shows if this is
useful. Also an MCU stripped down version that is client only is probably
somewhere down the line.
