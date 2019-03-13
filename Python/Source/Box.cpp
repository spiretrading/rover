#include "Box.hpp"

using namespace pybind11;
using namespace Rover;

void Rover::export_box(module& module) {
  export_box<object>(module, "");
}
