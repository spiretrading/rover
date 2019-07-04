#ifndef ROVER_PYTHON_RANDOM_PICK_HPP
#define ROVER_PYTHON_RANDOM_PICK_HPP
#include <pybind11/pybind11.h>

namespace Rover {

  //! Exports RandomPick generator.
  /*!
    \param module The module to export the class to.
  */
  void export_random_pick(pybind11::module& module);
}

#endif
