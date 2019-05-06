#include "LinearRegressionModel.hpp"

void Rover::export_linear_regression_model(pybind11::module& module) {
  export_linear_regression_model<PythonSample, double>(module, "");
}
