#!/bin/sh

# Absolute path to this script. /home/user/bin/foo.sh                           
SCRIPT=$(readlink -f $0)

# Absolute path this script is in. /home/user/bin                               
GIT_ROOT_PATH=`dirname ${SCRIPT}`/..

sudo apt-get update
sudo apt-get install cmake ninja-build make libboost-all-dev libconfig++-dev git git-gui gitk socat libev-dev libc-ares-dev daemon

# Get and build REACT-CPP
CURR_PWD=`pwd`
git clone https://github.com/CopernicaMarketingSoftware/REACT-CPP.git ${GIT_ROOT_PATH}/../reactcpp
cd ${GIT_ROOT_PATH}/../reactcpp
make
sudo make install
cd $CURR_PWD

git clone https://github.com/Microsoft/GSL.git ${GIT_ROOT_PATH}/../GSL

mkdir ${GIT_ROOT_PATH}/../serial_net-build
cd ${GIT_ROOT_PATH}/../serial_net-build
cmake -G Ninja ../serial_net
ninja -j 4

cd $CURR_PWD

