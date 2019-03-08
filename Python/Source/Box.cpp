#include "Box.hpp"

using namespace pybind11;
using namespace Rover;

bool Rover::Details::is_python_generator(const pybind11::object& arg) {
  return pybind11::hasattr(arg, "generate") &&
    PyCallable_Check(arg.attr("generate").ptr());
}

void Rover::export_box(module& module) {
  export_box<object>(module, "");
}
