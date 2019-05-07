#ifndef ROVER_PYTHON_SCALAR_VIEW_HPP
#define ROVER_PYTHON_SCALAR_VIEW_HPP
#include <functional>
#include <string_view>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "Rover/ScalarView.hpp"

namespace Rover {

  //! Exports an instantiation of the ScalarView class.
  /*!
    \param module The module to export the class to.
    \param type_name The name of the scalar type.
  */
  template<typename T>
  void export_scalar_view(pybind11::module& module, std::string_view
    type_name);

  //! Exports ScalarView<double> as the default ScalarView.
  void export_scalar_view(pybind11::module& module);

  template<typename T>
  void export_scalar_view(pybind11::module& module, std::string_view
      type_name) {
    using ComputationType = T;
    using ScalarSampleType = ScalarSample<ComputationType>;
    pybind11::class_<ScalarSampleType>(module, "ScalarSample")
      .def_readwrite("result", &ScalarSampleType::m_result)
      .def_readwrite("arguments", &ScalarSampleType::m_arguments);
    using GetterType = std::function<ScalarSampleType(std::size_t)>;
    using ScalarViewType = ScalarView<GetterType>;
    pybind11::class_<ScalarViewType>(module, "ScalarView")
      .def("__getitem__", &ScalarViewType::operator [])
      .def("__iter__", [](const ScalarViewType& t) {
         return pybind11::make_iterator(t.begin(), t.end());
       })
      .def("__len__", &ScalarViewType::size);
  }
}

#endif
