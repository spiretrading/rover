#ifndef ROVER_LINEAR_REGRESSION_HPP
#define ROVER_LINEAR_REGRESSION_HPP
#include <tuple>
#include <vector>
#include <dlib/matrix.h>

namespace Rover {

  //! Models a trial using linear regression.
  /*!
    \tparam T The arithmetic type used for calculations.
  */
  template<typename T = double>
  class LinearRegression {
    public:

      //! The arithmetic type used for calculations.
      using Type = T;

      //! Learns a trial represented by a ScalarView.
      template<typename ScalarView>
      void learn(const ScalarView& view);

      //! Predicts the dependent variable for a set of arguments.
      template<typename Arguments>
      Type predict(const Arguments& args) const;

    private:
      dlib::matrix<Type> m_transformation;

      template<typename ScalarView>
      static dlib::matrix<Type> compute_transformation_vector(
        const ScalarView& trial);
  };

  template<typename T>
  template<typename ScalarView>
  void LinearRegression<T>::learn(const ScalarView& view) {
    m_transformation = compute_transformation_vector(view);
  }

  template<typename T>
  template<typename Arguments>
  typename LinearRegression<T>::Type LinearRegression<T>::predict(const
      Arguments& args) const {
    auto x = dlib::matrix<Type, 1>(args.size() + 1);
    x(0, 0) = static_cast<Type>(1.);
    std::copy(args.begin(), args.end(), x.begin() + 1);
    auto result = x * m_transformation;
    return result;
  }

  template<typename T>
  template<typename ScalarView>
  dlib::matrix<typename LinearRegression<T>::Type> 
      LinearRegression<T>::compute_transformation_vector(const ScalarView&
      view) {
    auto x = dlib::matrix<Type>(view.size(), view[0].m_arguments.size() + 1);
    auto y = dlib::matrix<Type>(view.size(), 1);
    for(auto i = std::size_t(0); i < view.size(); ++i) {
      auto sample = view[i];
      x(i, 0) = static_cast<Type>(1.);
      std::copy(sample.m_arguments.begin(), sample.m_arguments.end(), x.begin()
        + i * x.nc() + 1);
      y(0, i) = sample.m_result;
    }
    auto xtr = dlib::trans(x);
    auto result = dlib::inv(xtr * x) * xtr * y;
    return result;
  }
}

#endif
