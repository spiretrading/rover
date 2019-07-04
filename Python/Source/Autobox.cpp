#include "Rover/Python/Autobox.hpp"

using namespace pybind11;

bool Rover::Details::is_python_generator(const object& arg) {
  return hasattr(arg, "generate") &&
    PyCallable_Check(arg.attr("generate").ptr());
}
