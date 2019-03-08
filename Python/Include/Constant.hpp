#ifndef ROVER_PYTHON_CONSTANT_HPP
#define ROVER_PYTHON_CONSTANT_HPP
#include <string_view>
#include <pybind11/pybind11.h>
#include "Rover/Box.hpp"
#include "Rover/Constant.hpp"

namespace Rover {

  //! Exports an instantiation of the Constant class.
  /*!
    \param module The module to export the class to.
    \param type_name The name of the type the Constant generates.
  */
  template<typename T>
  void export_constant(pybind11::module& module, std::string_view type_name);

  //! Exports a generic Constant for Python objects.
  void export_constant(pybind11::module& module);

  template<typename T>
  void export_constant(pybind11::module& module, std::string_view type_name) {
    auto name = std::string("Constant").append(type_name);
    pybind11::class_<Constant<T>>(module, name.c_str())
      .def(pybind11::init<T>())
      .def("generate", &Constant<T>::generate);
    pybind11::implicitly_convertible<Constant<T>, Box<T>>();
    pybind11::implicitly_convertible<Constant<T>, Box<pybind11::object>>();
    if constexpr(!std::is_same_v<T, pybind11::object>) {
      pybind11::implicitly_convertible<Constant<T>, Constant<pybind11::object>>();
    }
  }
}

#endif
