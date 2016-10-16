#!/bin/sh


#ip netns add client1

sudo killall serialnet
sudo killall socat

sudo socat system:"${HOME}/0_project/serial_net-build/src/mains/serialnet/serialnet --mode socat_tap -m -l 1 --address 1 --mtimeout 120 --wsdump /home/mikaelr/packet.txt --serial_device /dev/ttyUSB1" tun:192.168.128.1/24,iff-up,tun-name=tap0,tun-type=tap &

sudo ip link set dev tap0 mtu 500

#socat system:"${HOME}/0_project/serial_net-build/src/mains/serialnet/serialnet --mode socat_tun -l 1 --address 2 --wsdump ping_test.txt --serial_device /dev/ttyUSB1" tun:192.168.128.2/24,iff-up,tun-name=tun1 &

#ip link set dev tun1 netns client1

#sleep 1

#ping 192.168.128.2
