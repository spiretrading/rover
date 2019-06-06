#ifndef ROVER_MODEL_HPP
#define ROVER_MODEL_HPP
#include <type_traits>
#include "Rover/Basis.hpp"
#include "Rover/ScalarView.hpp"

namespace Rover { 

  //! Single-value regression (prediction) model.
  /*!
    \tparam A The type of the regression algorithm.
    \tparam T The type of the trial.
  */
  template<typename A, typename T>
  class Model {
    public:

      //! The type of the regression algorithm.
      using Algorithm = A;

      //! The computation type used by the algorithm.
      using ComputationType = typename Algorithm::Type;

      //! The type of the trial.
      using Trial = T;

      //! The type of a sample.
      using Sample = typename Trial::Sample;

      //! The type of a sample's arguments.
      using Arguments = typename Sample::Arguments;

      //! Creates a Model.
      /*!
        \param trial The trial.
        \param args The arguments for the algorithm's constructor.
      */
      template<typename... AlgoArgFwd>
      explicit Model(const Trial& trial, AlgoArgFwd&&... args);

      //! Predicts a Result for given arguments.
      auto operator ()(const Arguments& args) const;

    private:
      using Basis = Rover::Basis<Sample, ComputationType>;
      
      Basis m_basis;
      Algorithm m_algorithm;
  };

  template<typename A, typename T>
  template<typename... AlgoArgFwd>
  Model<A, T>::Model(const Trial& trial, AlgoArgFwd&&... args)
      : m_basis(trial),
        m_algorithm(std::forward<AlgoArgFwd>(args)...) {
    auto view = ScalarView([&](std::size_t i) {
      return m_basis.apply(trial[i]);
    }, trial.size());
    m_algorithm.learn(std::move(view));
  }

  template<typename A, typename T>
  auto Model<A, T>::operator ()(const Arguments& args) const {
    auto arguments = m_basis.apply(args);
    auto value = m_algorithm.predict(arguments);
    auto result = m_basis.restore_result(value);
    return result;
  }
}

#endif
