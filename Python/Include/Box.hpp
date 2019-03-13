#ifndef ROVER_PYTHON_BOX_HPP
#define ROVER_PYTHON_BOX_HPP
#include <string_view>
#include <pybind11/pybind11.h>
#include "Autobox.hpp"
#include "Rover/Box.hpp"

namespace Rover {

  //! Exports an instantiation of the Box class.
  /*!
    \param module The module to export the class to.
    \param type_name The name of the type the Box generates.
  */
  template<typename T>
  void export_box(pybind11::module& module, std::string_view type_name);

  //! Exports a generic Box for Python objects.
  void export_box(pybind11::module& module);

  template<typename T>
  void export_box(pybind11::module& module, std::string_view type_name) {
    auto name = std::string("Box").append(type_name);
    pybind11::class_<Box<T>>(module, name.c_str())
      .def(init(
        [](pybind11::object arg) {
          return python_autobox<T>(std::move(arg));
        }))
      .def("generate", &Box<T>::generate);
    if constexpr(!std::is_same_v<T, pybind11::object>) {
      pybind11::implicitly_convertible<Box<T>, Box<pybind11::object>>();
      pybind11::implicitly_convertible<Box<pybind11::object>, Box<T>>();
    }
  }
}

#endif
