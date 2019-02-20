#ifndef ROVER_PYTHON_EVALUATOR_HPP
#define ROVER_PYTHON_EVALUATOR_HPP
#include <pybind11/pybind11.h>

namespace Rover {

  //! Exports the Evaluator class.
  void export_evaluator(pybind11::module& module);
}

#endif
