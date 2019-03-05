#include "Evaluator.hpp"
#include "Rover/Box.hpp"
#include "Rover/Evaluator.hpp"

using namespace pybind11;
using namespace Rover;

void Rover::export_evaluator(module& module) {
  pybind11::class_<Evaluator>(module, "Evaluator")
    .def(pybind11::init<>())
    .def("evaluate", &Evaluator::evaluate<Box<pybind11::object>>);
}
