#ifndef ROVER_LINEAR_REGRESSION_MODEL_HPP
#define ROVER_LINEAR_REGRESSION_MODEL_HPP
#include <tuple>
#include <vector>
#include <dlib/matrix.h>

namespace Rover {

  //! Models a trial using linear regression.
  /*!
    \tparam T The Trial to model.
    \tparam C The arithmetic type used for calculations.
    \detail Every argument and the result of the Trial must be convertible to C
    \detail C must be convertible to Trial's result
  */
  template<typename T, typename C = double>
  class LinearRegressionModel {
    public:

      //! The Trial to model.
      using Trial = T;

      //! The arithmetic type used for calculations
      using CompType = C;

      //! The result of the trial.
      using Result = typename Trial::Sample::Result;

      //! Constructs a linear regression model for a trial.
      /*!
        \param trial The trial to model.
      */
      LinearRegressionModel(const Trial& trial);

      template<typename... Arg>
      Result operator ()(Arg&&... arg) const;

    private:
      static dlib::matrix<CompType> compute_transformation_vector(
        const Trial& trial);
      dlib::matrix<CompType> m_transformation;
  };

  template<typename T, typename C>
  LinearRegressionModel<T, C>::LinearRegressionModel(const Trial& trial)
    : m_transformation(compute_transformation_vector(trial)) {}

  template<typename T, typename C>
  template<typename... Arg>
  typename LinearRegressionModel<T, C>::Result
      LinearRegressionModel<T, C>::operator ()(Arg&&... arg) const {
    auto x = dlib::matrix<CompType, 1, std::tuple_size_v<
      typename Trial::Sample::Parameters> + 1>();
    auto coef = std::vector<CompType>();
    coef.reserve(std::tuple_size_v<typename Trial::Sample::Parameters>);
    (coef.push_back(std::forward<Arg>(arg)), ...);
    x(0, 0) = static_cast<CompType>(1.);
    std::move(coef.begin(), coef.end(), x.begin() + 1);
    auto result = static_cast<Result>(x * m_transformation);
    return result;
  }

  template<typename T, typename C>
  dlib::matrix<typename LinearRegressionModel<T, C>::CompType> 
      LinearRegressionModel<T, C>::compute_transformation_vector(
      const Trial& trial) {
    auto x = dlib::matrix<CompType>(trial.size(),
      std::tuple_size_v<typename Trial::Sample::Parameters> + 1);
    for(auto i = 0U; i < trial.size(); ++i) {
      auto row = std::apply([](const auto&... param) {
        return std::vector<CompType>{ static_cast<CompType>(1.),
          static_cast<CompType>(param)... };
      }, trial[i].m_arguments);
      std::move(row.begin(), row.end(), x.begin() + i * x.nc());
    }
    auto y = dlib::matrix<CompType>(trial.size(), 1);
    std::transform(trial.begin(), trial.end(), y.begin(),
      [](const auto& sample) {
        return static_cast<CompType>(sample.m_result);
      });
    auto xtr = dlib::trans(x);
    auto result = dlib::inv(xtr * x) * xtr * y;
    return result;
  }
}

#endif
