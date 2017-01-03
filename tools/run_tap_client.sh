#!/bin/sh

sudo killall serialnet

sudo ip tuntap add name tap0 mode tap
#sudo ifconfig tap0 192.168.128.2 up

sudo ${HOME}/0_project/serial_net-build/src/mains/serialnet/serialnet --mode tap -l 1 --wsdump ${HOME}/packet.txt -d /dev/ttyUSB0 --serial-options pulldown

#sudo ip link set dev tap0 mtu 500
