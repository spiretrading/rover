#include "Model.hpp"
#include "Sample.hpp"
#include "TrialView.hpp"

using namespace Rover;

void Rover::export_model(pybind11::module& module) {
  export_model<TrialView<PythonSample>, double>(module, "");
}
