#include "Rover/Python/ListTrial.hpp"
#include "Rover/Python/Sample.hpp"

using namespace pybind11;
using namespace Rover;

void Rover::export_list_trial(module& module) {
  class_<ListTrial<PythonSample>>(module, "ListTrial")
    .def(init<>())
    .def("reserve", &ListTrial<PythonSample>::reserve)
    .def("capacity", &ListTrial<PythonSample>::capacity)
    .def("insert", static_cast<void (ListTrial<PythonSample>::*)(const
       PythonSample&)>(&ListTrial<PythonSample>::insert))
    .def("insert", [](ListTrial<PythonSample>& out,
         const ListTrial<PythonSample>& in) {
       out.insert(in.begin(), in.end());
     })
    .def("__getitem__", &ListTrial<PythonSample>::operator [])
    .def("__iter__", [](const ListTrial<PythonSample>& t) {
       return make_iterator(t.begin(), t.end());
     })
    .def("__len__", &ListTrial<PythonSample>::size);
}
