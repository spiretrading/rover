#ifndef ROVER_PYTHON_PICK_HPP
#define ROVER_PYTHON_PICK_HPP
#include <vector>
#include <pybind11/pybind11.h>
#include "Rover/Box.hpp"
#include "Rover/Evaluator.hpp"

namespace Rover {

  //! Exports Pick generator.
  /*!
    \param module The module to export the class to.
  */
  void export_pick(pybind11::module& module);

  //! Python implementation of the Pick generator.
  class PythonPick {
    public:

      //! The type of generated values.
      using Type = pybind11::object;

      //! Constructs Pick.
      /*!
        \param choice Function returning the index of the next generator to
                      evaluate.
        \param generators One or more generators evaluating to the same type.
      */
      PythonPick(pybind11::object choice, pybind11::args generators);

      //! Evaluates the generator.
      Type generate(Evaluator& evaluator);

    private:
      pybind11::object m_choice;
      std::vector<Box<pybind11::object>> m_generators;
  };  
}

#endif
