#include "Rover/Python/LinearRegressionModel.hpp"
#include "Rover/TrialView.hpp"

using namespace pybind11;
using namespace Rover;

void Rover::export_linear_regression_model(module& module) {
  export_linear_regression_model<TrialView<PythonSample>, double>(module, "");
}
