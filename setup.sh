#!/bin/bash
if [ ! -d "Dependencies" ]; then
  mkdir Dependencies
fi
pushd Dependencies
if [ ! -d "Catch2-2.6.1" ]; then
  git clone --branch v2.6.1 https://github.com/catchorg/Catch2.git Catch2-2.6.1
  if [ -d "Catch2-2.6.1" ]; then
    pushd Catch2-2.6.1
    cmake .
    make
    make install
    popd
  fi
fi
if [ ! -d "pybind11-2.2.4" ]; then
  git clone --branch v2.2.4 https://github.com/pybind/pybind11.git pybind11-2.2.4
  if [ -d "pybind11-2.2.4" ]; then
    pushd pybind11-2.2.4
    cmake .
    make
    make install
    popd
  fi
fi
popd

