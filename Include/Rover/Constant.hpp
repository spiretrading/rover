#ifndef ROVER_CONSTANT_HPP
#define ROVER_CONSTANT_HPP
#include <type_traits>

namespace Rover {

  /** An argument generator that evaluates to a constant value. */
  template<typename T>
  class Constant {
    public:

      /** The type of constant to evaluate. */
      using Type = T;

      /** Constructs a Constant generator from a value. */
      template<typename U>
      constexpr Constant(U&& value);

      template<typename Session>
      constexpr Type generate(Session& session) const;

      auto build_session() const;
  };

  template<typename U>
  Constant(U&&) -> Constant<std::decay_t<U>>;
}

#endif
