#ifndef ROVER_CONSTANT_HPP
#define ROVER_CONSTANT_HPP
#include <utility>
#include "Rover/Evaluator.hpp"

namespace Rover {

  /** An argument generator that evaluates to a constant value. */
  template<typename T>
  class Constant {
    public:

      /** The type of constant to evaluate. */
      using Type = T;

      /** Constructs a constant generator from a value. */
      constexpr Constant(T value);

      constexpr Type generate(Evaluator& evaluator) const;

    private:
      Type m_value;
  };

  template<typename T>
  constexpr Constant<T>::Constant(T value)
      : m_value(std::move(value)) {}

  template<typename T>
  constexpr typename Constant<T>::Type Constant<T>::generate(
      Evaluator& evaluator) const {
    return m_value;
  }
}

#endif
