#include "Rover/Python/Constant.hpp"

using namespace pybind11;
using namespace Rover;

void Rover::export_constant(module& module) {
  export_constant<object>(module, "");
}
