#include "Rover/Python/ScalarView.hpp"

using namespace pybind11;
using namespace Rover;

void Rover::export_scalar_view(module& module) {
  export_scalar_view<double>(module, "");
}
