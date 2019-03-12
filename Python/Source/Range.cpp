#include "Range.hpp"

using namespace pybind11;
using namespace Rover;

void Rover::export_range(module& module) {
  class_<Details::PythonRange>(module, "Range")
    .def(init<object, object>())
    .def(init<object, object, object>())
    .def("generate", &Details::PythonRange::generate);

  implicitly_convertible<Details::PythonRange, Box<object>>();
}
