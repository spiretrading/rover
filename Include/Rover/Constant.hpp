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
      template<typename U>
      constexpr Constant(U&& value);

      constexpr Type generate(Evaluator& evaluator) const;

    private:
      Type m_value;
  };

  template<typename U>
  Constant(U&&) -> Constant<std::decay_t<U>>;

  template<typename T>
  template<typename U>
  constexpr Constant<T>::Constant(U&& value)
      : m_value(std::forward<U>(value)) {}

  template<typename T>
  constexpr typename Constant<T>::Type Constant<T>::generate(
      Evaluator& evaluator) const {
    return m_value;
  }
}

#endif
