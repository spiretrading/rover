#include "ListTrial.hpp"
#include "Sample.hpp"

using namespace pybind11;

namespace Rover {
  void export_list_trial(pybind11::module& module) {
    class_<ListTrial<PythonSample>>(module, "ListTrial")
      .def(init<>())
      .def("reserve", &ListTrial<PythonSample>::reserve)
      .def("capacity", &ListTrial<PythonSample>::capacity)
      .def("insert", static_cast<void (ListTrial<PythonSample>::*)(const
         PythonSample&)>(&ListTrial<PythonSample>::insert))
      .def("insert", [](ListTrial<PythonSample>& out,
           ListTrial<PythonSample> in) {
         out.insert(std::make_move_iterator(in.begin()),
           std::make_move_iterator(in.end()));
       })
      .def("__getitem__", &ListTrial<PythonSample>::operator [])
      .def("__iter__", [](const ListTrial<PythonSample>& t) {
         return make_iterator(t.begin(), t.end());
       })
      .def("__len__", &ListTrial<PythonSample>::size);
  }
}
