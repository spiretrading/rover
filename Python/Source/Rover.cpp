#include <pybind11/pybind11.h>
#include "Box.hpp"
#include "Constant.hpp"
#include "Evaluator.hpp"

using namespace pybind11;
using namespace Rover;

PYBIND11_MODULE(rover, module) {
  export_evaluator(module);
  export_box(module);
  export_box<int>(module, "Int");
  export_box<long>(module, "Long");
  export_box<float>(module, "Float");
  export_box<double>(module, "Double");
  export_box<unsigned>(module, "Unsigned");
  export_box<unsigned long>(module, "UnsignedLong");
  export_constant(module);
  export_constant<int>(module, "Int");
  export_constant<long>(module, "Long");
  export_constant<float>(module, "Float");
  export_constant<double>(module, "Double");
  export_constant<unsigned>(module, "Unsigned");
  export_constant<unsigned long>(module, "UnsignedLong");
}
