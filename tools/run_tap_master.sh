#!/bin/sh

sudo killall serialnet

#sudo ip tuntap add name tap0 mode tap
#sudo ifconfig tap0 192.168.128.1

sudo ${HOME}/0_project/serial_net-build/src/mains/serialnet/serialnet --mode tap -m -l 1 --wsdump ${HOME}/packet.txt -d /dev/ttyUSB0 --serial-options pulldown

# --mtimeout 120

#sudo gdb ${HOME}/0_project/serial_net-build/src/mains/serialnet/serialnet -ex 'run --mode tap -m -l 2 --address 1 --mtimeout 3 --wsdump /home/mikaelr/packet.txt -d /dev/ttyUSB0'

#sudo ip link set dev tap0 mtu 500
