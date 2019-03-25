#ifndef ROVER_PYTHON_LIST_TRIAL_HPP
#define ROVER_PYTHON_LIST_TRIAL_HPP
#include <string_view>
#include <pybind11/pybind11.h>
#include "Rover/ListTrial.hpp"
#include "Rover/Sample.hpp"

namespace Rover {
  
  //! Exports a Trial for Python object samples
  /*!
    \param module The module to export the class to
  */
  void export_list_trial(pybind11::module& module);

  //! Exports a Trial for a particular Sample type
  /*!
    \param module The module to export the class to
    \param suffix The suffix to the name of the exported type
  */
  template<typename S>
  void export_list_trial(pybind11::module& module, std::string_view suffix);

  template<typename S>
  void export_list_trial(pybind11::module& module, std::string_view suffix) {
    auto name = std::string("ListTrial").append(suffix);
    pybind11::class_<ListTrial<S>>(module, name.c_str())
      .def(pybind11::init<>())
      .def("reserve", &ListTrial<S>::reserve)
      .def("capacity", &ListTrial<S>::capacity)
      .def("insert", (void (ListTrial<S>::*)(const S&))
         &ListTrial<S>::insert)
      .def("insert", [](ListTrial<S>& out,
           ListTrial<S> in) {
         out.insert(std::make_move_iterator(in.begin()),
           std::make_move_iterator(in.end()));
       })
      .def("__getitem__", &ListTrial<S>::operator [])
      .def("__iter__", [](const ListTrial<S>& t) {
         return pybind11::make_iterator(t.begin(), t.end());
       })
      .def("__len__", &ListTrial<S>::size);
    pybind11::implicitly_convertible<ListTrial<S>, ListTrial<PythonSample>>();
    pybind11::implicitly_convertible<ListTrial<PythonSample>, ListTrial<S>>();
  }
}

#endif
