#ifndef ROVER_PYTHON_LINEAR_REGRESSION_MODEL_HPP
#define ROVER_PYTHON_LINEAR_REGRESSION_MODEL_HPP
#include <string_view>
#include <pybind11/pybind11.h>
#include "Rover/LinearRegression.hpp"
#include "Rover/Model.hpp"
#include "Rover/TrialView.hpp"
#include "Arithmetics.hpp"
#include "Sample.hpp"
#include "Scalar.hpp"

namespace Rover {

  void export_linear_regression_model(pybind11::module& module);

  template<typename S, typename T>
  void export_linear_regression_model(pybind11::module& module,
    std::string_view suffix);

  template<typename S, typename T>
  void export_linear_regression_model(pybind11::module& module,
      std::string_view suffix) {
    using ModelType = Model<LinearRegression<PythonScalar<T>>, TrialView<S>>;
    auto name = std::string("LinearRegressionModel").append(suffix);
    pybind11::class_<ModelType>(module, name.c_str())
      .def(pybind11::init<const TrialView<S>&>())
      .def("__call__", &ModelType::operator ());
  }
}

#endif
