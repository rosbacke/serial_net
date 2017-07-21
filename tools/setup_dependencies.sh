#!/bin/sh

# Absolute path to this script. /home/user/bin/foo.sh                           
SCRIPT=$(readlink -f $0)

# Absolute path this script is in. /home/user/bin                               
GIT_ROOT_PATH=`dirname ${SCRIPT}`/..

sudo apt-get update
sudo apt-get install cmake ninja-build make libboost-all-dev libconfig++-dev git git-gui gitk socat libev-dev libc-ares-dev libmosquitto-dev

git -C ${GIT_ROOT_PATH} submodule update --init external/googletest

mkdir ${GIT_ROOT_PATH}/out
cd ${GIT_ROOT_PATH}/out
cmake -G Ninja ..
ninja -j 4

cd $CURR_PWD
