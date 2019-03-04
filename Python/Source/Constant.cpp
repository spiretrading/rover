#include "Constant.hpp"

using namespace pybind11;
using namespace Rover;

void Rover::export_constant(module& module) {
  pybind11::class_<Constant<pybind11::object>>(module, "Constant")
    .def(pybind11::init<pybind11::object>())
    .def("generate", &Constant<pybind11::object>::generate);
}
