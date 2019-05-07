#include "Rover/TrialView.hpp"
#include "LinearRegressionModel.hpp"

void Rover::export_linear_regression_model(pybind11::module& module) {
  export_linear_regression_model<TrialView<PythonSample>, double>(module, "");
}
