#!/bin/bash
if [ ! -d "Dependencies" ]; then
  mkdir Dependencies
fi
pushd Dependencies
if [ ! -d "Catch2-2.2.1" ]; then
  git clone --branch v2.2.1 https://github.com/catchorg/Catch2.git Catch2-2.2.1
  if [ -d "Catch2-2.2.1" ]; then
    pushd Catch2-2.2.1
    cmake .
    make
    make install
    popd
  fi
fi
popd
