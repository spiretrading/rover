#ifndef ROVER_PYTHON_LINEAR_REGRESSION_MODEL_HPP
#define ROVER_PYTHON_LINEAR_REGRESSION_MODEL_HPP
#include <string_view>
#include <pybind11/pybind11.h>
#include "Rover/LinearRegression.hpp"
#include "Rover/Model.hpp"
#include "Arithmetics.hpp"
#include "Basis.hpp"
#include "Sample.hpp"
#include "Scalar.hpp"
#include "VariableTraits.hpp"

namespace Rover {

  //! Exports Model<LinearRegression<PythonScalar<double>>,
  //! TrialView<PythonSample>> as the default Model.
  /*
    \param module The module to export the class to.
  */
  void export_linear_regression_model(pybind11::module& module);

  //! Exports Model<LinearRegression<PythonScalar<S>>, T>.
  /*
    \tparam T The type of the trial.
    \tparam S The raw arithmetic type of the scalar.
    \param module The module to export the class to.
    \param suffix The suffix to the name of the exported class.
  */
  template<typename T, typename S>
  void export_linear_regression_model(pybind11::module& module,
    std::string_view suffix);

  template<typename T, typename S>
  void export_linear_regression_model(pybind11::module& module,
      std::string_view suffix) {
    using ModelType = Model<LinearRegression<PythonScalar<S>>, T>;
    auto name = std::string("LinearRegressionModel").append(suffix);
    pybind11::class_<ModelType>(module, name.c_str())
      .def(pybind11::init<const T&>())
      .def("__call__", &ModelType::operator ());
  }
}

#endif
