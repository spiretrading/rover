#include "Rover/Generator.hpp"
#include "Box.hpp"
#include "Generator.hpp"

using namespace pybind11;
using namespace Rover;

void Rover::export_generate(module& module) {
  export_generate<Box<object>>(module, "generate");
}
