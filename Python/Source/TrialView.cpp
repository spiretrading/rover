#include "TrialView.hpp"

using namespace pybind11;
using namespace Rover;

void Rover::export_trial_view(module& module) {
  class_<Details::NativeTrialProxy>(module, "_NativeTrialProxy")
    .def(init([](const object& obj) {
       return Details::NativeTrialProxy(obj);
     }), keep_alive<2, 1>());
  class_<TrialView<PythonSample>>(module, "TrialView")
    .def(init<const ListTrial<PythonSample>&>())
    .def(init([](const Details::NativeTrialProxy& proxy) {
       return TrialView<PythonSample>(proxy);
     }), keep_alive<2, 1>())
    .def("__getitem__", &TrialView<PythonSample>::operator [])
    .def("__iter__", [](const TrialView<PythonSample>& v) {
       return make_iterator(v.begin(), v.end());
     })
    .def("__len__", &TrialView<PythonSample>::size);
  pybind11::implicitly_convertible<ListTrial<PythonSample>, TrialView<
    PythonSample>>();
}
