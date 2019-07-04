#include "Rover/Python/Generator.hpp"
#include "Rover/Python/Box.hpp"
#include "Rover/Generator.hpp"

using namespace pybind11;
using namespace Rover;

void Rover::export_generate(module& module) {
  export_generate<Box<object>>(module, "generate");
}
