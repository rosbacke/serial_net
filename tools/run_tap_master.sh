#!/bin/sh

sudo killall serialnet

export TAP_USER=${USER}
sudo ip tuntap add name tap0 mode tap user ${TAP_USER}

# Use the following lines the first time to set up a tap interface belonging to
# the current user. You can use the serialnet mode 'setup_tap' to accomplish
# the same task. This allows using serialnet without sudo privileges when
# routing traffic. Add the current user to group 'dialout' to get access
# to serial ports also.
export MY_USER=$USER
sudo ip tuntap add tap0 mode tap user $MY_USER

# Set up IP address. (TODO: support for DHCP instead)
sudo ifconfig tap0 192.168.128.1
sudo ip link set dev tap0 mtu 500
SERIAL=/dev/ttyUSB0

${HOME}/0_project/serial_net/out/src/mains/serialnet/serialnet --mode tap -m -l 0 --wsdump ${HOME}/packet.txt -d ${SERIAL} --serial-options pulldown

# --mtimeout 120

#gdb ${HOME}/0_project/serial_net/out/src/mains/serialnet/serialnet -ex 'run --mode tap -m -l 0 --address 1 --wsdump /home/mikaelr/packet.txt -d /dev/ttyS1'
#  --mtimeout 3

#sudo ip link set dev tap0 mtu 500
