#include "Autobox.hpp"

namespace Rover::Details {
  bool is_python_generator(const pybind11::object& arg) {
    return pybind11::hasattr(arg, "generate") &&
      PyCallable_Check(arg.attr("generate").ptr());
  }
}
