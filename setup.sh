#!/bin/bash
let cores="`grep -c "processor" < /proc/cpuinfo`"
root="$(pwd)"
if [ ! -d "Catch2-2.6.1" ]; then
  git clone --branch v2.6.1 https://github.com/catchorg/Catch2.git Catch2-2.6.1
fi
if [ ! -d "dlib-19.17" ]; then
  wget https://github.com/davisking/dlib/archive/v19.17.tar.gz -O dlib-v19.17.tar.gz --no-check-certificate
  tar -xzf dlib-v19.17.tar.gz
  pushd dlib-19.17
  mkdir Build
  mkdir Install
  pushd Install
  mkdir Debug
  mkdir Release
  popd
  pushd Build
  mkdir Debug
  pushd Debug
  cmake ../../dlib -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../../Install/Debug -DENABLE_ASSERTS=OFF
  cmake --build . --target install
  popd
  mkdir Release
  pushd Release
  cmake ../../dlib -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../../Install/Release -DENABLE_ASSERTS=OFF
  cmake --build . --target install
  popd
  popd
  popd
  rm dlib-v19.17.tar.gz
fi
if [ ! -d "pybind11-2.2.4" ]; then
  git clone --branch v2.2.4 https://github.com/pybind/pybind11.git pybind11-2.2.4
fi
if [ ! -d "Python-3.6.7" ]; then
  wget https://www.python.org/ftp/python/3.6.7/Python-3.6.7.tgz --no-check-certificate
  if [ -f Python-3.6.7.tgz ]; then
    gzip -d -c Python-3.6.7.tgz | tar -xf -
    pushd Python-3.6.7
    export CFLAGS="-fPIC"
    ./configure --prefix="$root/Python-3.6.7"
    make -j $cores
    make install
    unset CFLAGS
    popd
    rm Python-3.6.7.tgz
  fi
fi
