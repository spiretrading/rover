#ifndef ROVER_PYTHON_CONSTANT_HPP
#define ROVER_PYTHON_CONSTANT_HPP
#include <string_view>
#include <pybind11/pybind11.h>
#include "Rover/Constant.hpp"

namespace Rover {

  //! Exports an instantiation of the Constant class.
  /*!
    \param module The module to export the class to.
    \param type_name The name of the type the Constant generates.
  */
  template<typename T>
  void export_constant(pybind11::module& module, std::string_view type_name) {}

  //! Exports a generic Constant for Python objects.
  void export_constant(pybind11::module& module);
}

#endif
