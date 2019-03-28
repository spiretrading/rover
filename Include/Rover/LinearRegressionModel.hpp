#ifndef ROVER_LINEAR_REGRESSION_MODEL_HPP
#define ROVER_LINEAR_REGRESSION_MODEL_HPP
#include <tuple>
#include <vector>
#include <dlib/matrix.h>

namespace Rover {

  //! Models a trial using linear regression.
  /*!
    \tparam T The Trial to model.
  */
  template<typename T>
  class LinearRegressionModel {
    public:

      //! The Trial to model.
      using Trial = T;

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
      static dlib::matrix<Result> compute_transformation_vector(
        const Trial& trial);
      dlib::matrix<Result> m_transformation;
  };

  template<typename T>
  LinearRegressionModel<T>::LinearRegressionModel(const T& trial)
    : m_transformation(compute_transformation_vector(trial)) {}

  template<typename T>
  template<typename... Arg>
  typename LinearRegressionModel<T>::Result
      LinearRegressionModel<T>::operator ()(Arg&&... arg) const {
    auto x = dlib::matrix<Result, 1, std::tuple_size_v<
      typename Trial::Sample::Parameters> + 1>();
    auto coef = std::vector<Result>();
    coef.reserve(std::tuple_size_v<typename Trial::Sample::Parameters>);
    (coef.push_back(std::forward<Arg>(arg)), ...);
    x(0, 0) = 1.;
    std::move(coef.begin(), coef.end(), x.begin() + 1);
    auto result = x * m_transformation;
    return result;
  }

  template<typename T>
  dlib::matrix<typename LinearRegressionModel<T>::Result> 
      LinearRegressionModel<T>::compute_transformation_vector(
      const Trial& trial) {
    auto x = dlib::matrix<Result>(trial.size(),
      std::tuple_size_v<typename Trial::Sample::Parameters> + 1);
    for(auto i = 0U; i < trial.size(); ++i) {
      auto row = std::apply([](const auto&... param) {
        return std::vector<Result>{ 1., param... };
      }, trial[i].m_arguments);
      std::move(row.begin(), row.end(), x.begin() + i * x.nc());
    }
    auto y = dlib::matrix<Result>(trial.size(), 1);
    std::transform(trial.begin(), trial.end(), y.begin(),
      [](const auto& sample) {
        return sample.m_result;
      });
    auto xtr = dlib::trans(x);
    auto result = dlib::inv(xtr * x) * xtr * y;
    return result;
  }
}

#endif
