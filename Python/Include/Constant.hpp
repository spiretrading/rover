#ifndef ROVER_PYTHON_CONSTANT_HPP
#define ROVER_PYTHON_CONSTANT_HPP
#include <pybind11/pybind11.h>
#include "Rover/Constant.hpp"

namespace Rover {

  //! Exports an instantiation of the Constant class.
  template<typename T>
  void export_constant(pybind11::module& module) {}

  //! Exports a generic Constant for Python objects.
  template<>
  void export_constant<pybind11::object>(pybind11::module& module);
}

#endif
