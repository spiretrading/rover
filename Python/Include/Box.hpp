#ifndef ROVER_PYTHON_BOX_HPP
#define ROVER_PYTHON_BOX_HPP
#include <pybind11/pybind11.h>
#include "Rover/Box.hpp"

namespace Rover {

  //! Exports an instantiation of the Box class.
  template<typename T>
  void export_box(pybind11::module& module) {}

  //! Exports a generic Box for Python objects.
  template<>
  void export_box<pybind11::object>(pybind11::module& module);
}

#endif
