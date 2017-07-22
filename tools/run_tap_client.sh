#!/bin/sh

sudo killall serialnet

export TAP_USER=${USER}
sudo ip tuntap add name tap0 mode tap user ${TAP_USER}
sudo ifconfig tap0 192.168.128.2 up
sudo ip link set dev tap0 mtu 500

${HOME}/0_project/serial_net/out/src/mains/serialnet/serialnet --mode tap -l 0 --wsdump ${HOME}/packet.txt -d /dev/ttyUSB0
