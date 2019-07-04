#ifndef ROVER_PYTHON_GENERATOR_HPP
#define ROVER_PYTHON_GENERATOR_HPP
#include <pybind11/pybind11.h>
#include "Rover/Generator.hpp"

namespace Rover {
  
  //! Exports generate function for Box<pybind11::object>.
  /*!
    \param module The module to export the function to.
  */
  void export_generate(pybind11::module& module);

  //! Exports generate function for a selected generator type.
  /*!
    \tparam T The type of the generator.
    \param function_name The name of the exported function.
  */
  template<typename T>
  void export_generate(pybind11::module& module, const char* function_name) {
    module.def(function_name, [](T& generator) {
      return generate<T>(generator);
    });
  }
}

#endif
