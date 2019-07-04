#ifndef ROVER_PYTHON_VARIABLE_TRAITS_HPP
#define ROVER_PYTHON_VARIABLE_TRAITS_HPP
#include <pybind11/pybind11.h>
#include "Rover/VariableTraits.hpp"
#include "Arithmetics.hpp"

namespace Rover {

  //! A struct wrapper to check whether a python object is continuous for
  //! modeling purposes for a given scalar type.
  /*!
    \tparam S The type of the scalar.
  */
  template<typename S>
  struct ContinuousChecker<S, pybind11::object> {

    //! Checks whether a python object is continuous for modeling purposes for
    //! scalar S.
    static bool check(const pybind11::object& obj);
  };

  template<typename S>
  bool ContinuousChecker<S, pybind11::object>::check(const pybind11::object&
      obj) {
    try {
      obj + obj;
      -obj;
      obj == obj;
      obj != obj;
      obj < obj;
      S() * obj;
      try {
        obj / obj;
      } catch(pybind11::error_already_set& e) {
        e.restore();
        if(PyErr_ExceptionMatches(PyExc_ZeroDivisionError)) {
          PyErr_Clear();
        } else {
          return false;
        }
      }
      return true;
    } catch(const pybind11::error_already_set&) {
      return false;
    }
  }
}

#endif
