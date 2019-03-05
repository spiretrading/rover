#include "Box.hpp"
#include "Rover/Constant.hpp"

using namespace pybind11;
using namespace Rover;

void Rover::export_box(module& module) {
  pybind11::class_<Box<pybind11::object>>(module, "Box")
    .def(pybind11::init<Constant<pybind11::object>>())
    .def("generate", &Box<pybind11::object>::generate);
}