#ifndef ROVER_PYTHON_SELECT_HPP
#define ROVER_PYTHON_SELECT_HPP
#include <pybind11/pybind11.h>

namespace Rover {

  //! Exports an instantiation of the Select class.
  /*!
    \param module The module to export the class to.
  */
  void export_select(pybind11::module& module);
}

#endif
