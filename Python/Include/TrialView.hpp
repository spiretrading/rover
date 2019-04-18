#ifndef ROVER_PYTHON_TRIAL_VIEW_HPP
#define ROVER_PYTHON_TRIAL_VIEW_HPP
#include <string_view>
#include <type_traits>
#include <pybind11/pybind11.h>
#include "Rover/ListTrial.hpp"
#include "Rover/TrialView.hpp"
#include "Sample.hpp"

namespace Rover {

  //! Exports the TrialView for PythonSample.
  /*!
    \param module The module to export the class to.
  */
  void export_trial_view(pybind11::module& module);

  //! Exports the TrialView for a particular Sample type.
  /*!
    \param module The module to export the class to.
    \param type_name The name of the Sample type.
  */
  template<typename S>
  void export_trial_view(pybind11::module& module, std::string_view type_name);

  template<typename S>
  void export_trial_view(pybind11::module& module, std::string_view
      type_name) {
    auto name = std::string("TrialView").append(type_name);
    pybind11::class_<TrialView<S>>(module, name.c_str())
      .def(pybind11::init<const ListTrial<S>&>())
      .def("__getitem__", &TrialView<S>::operator [])
      .def("__iter__", [](const TrialView<S>& v) {
         return pybind11::make_iterator(v.begin(), v.end());
       })
      .def("__len__", &TrialView<S>::size);
    if constexpr(!std::is_same_v<S, PythonSample>) {
      pybind11::implicitly_convertible<TrialView<S>, TrialView<PythonSample>>();
      pybind11::implicitly_convertible<TrialView<PythonSample>, TrialView<S>>();
    }
  }
}

#endif
