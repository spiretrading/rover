#ifndef ROVER_CONSTANT_HPP
#define ROVER_CONSTANT_HPP
#include <type_traits>
#include "Generator.hpp"
#include "Noncopyable.hpp"

namespace Rover {

  /** An argument generator that evaluates to a constant value. */
  template<typename T>
  class Constant : Noncopyable {
    static_assert(!implements_concept_v<T, Generator>,
      "Attempt to create Constant of another Generator");

    public:

      /** The type of constant to evaluate. */
      using Type = T;

      /** Constructs a Constant generator from a value. */
      template<typename U, typename std::enable_if_t<!std::is_same_v<U,
        Constant<T>>>* = nullptr>
      constexpr Constant(U&& value);

      template<typename Session>
      constexpr Type generate(Session& session) const;

      auto build_session() const;

    private:
      Type m_value;
  };

  template<typename U>
  Constant(U&&) -> Constant<std::decay_t<U>>;

  template<typename T>
  template<typename U, typename std::enable_if_t<!std::is_same_v<U,
    Constant<T>>>*>
  constexpr Constant<T>::Constant(U&& value)
    : m_value(std::forward<U>(value)) {}

  template<typename T>
  template<typename Session>
  typename constexpr Constant<T>::Type Constant<T>::generate(
    Session& session) const {
    return m_value; 
  }

  template<typename T>
  auto Constant<T>::build_session() const {
    return std::make_tuple();
  }

  template<typename Type>
  struct ImplementsConcept<Constant<Type>, Generator> : std::true_type {};

}

#endif
