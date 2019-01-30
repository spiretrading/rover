#!/bin/bash
let cores="`grep -c "processor" < /proc/cpuinfo`"
directory=$(dirname $(readlink -f $0))
username=$(logname 2>/dev/null || echo ${SUDO_USER:-${USER}})

apt-get install -y automake cmake g++ gcc gdb git make parallel zip
ldconfig

if [ ! -d "Dependencies" ]; then
  mkdir Dependencies
fi
pushd Dependencies
if [ ! -d "Catch2-2.2.1" ]; then
  sudo -u $username git clone --branch v2.2.1 https://github.com/catchorg/Catch2.git Catch2-2.2.1
  if [ -d "Catch2-2.2.1" ]; then
    pushd Catch2-2.2.1
    sudo -u $username cmake .
    sudo -u $username make
    make install
    popd
  fi
fi
popd
