#include "Rover/Python/Model.hpp"
#include "Rover/Python/Sample.hpp"
#include "Rover/Python/TrialView.hpp"

using namespace pybind11;
using namespace Rover;

void Rover::export_model(module& module) {
  export_model<TrialView<PythonSample>, double>(module, "");
}
