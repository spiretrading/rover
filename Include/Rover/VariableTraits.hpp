#ifndef ROVER_VARIABLE_TRAITS_HPP
#define ROVER_VARIABLE_TRAITS_HPP
#include <type_traits>

namespace Rover {


  //! Checks whether a variable is continuous for modeling purposes for a given
  //! scalar type.
  /*!
    \tparam Scalar The type of the scalar.
    \tparam Variable The type of the variable.
    \param variable The variable.
    \details The variable is considered continuous if the following conditions
             are met (S = Scalar, V = Variable):
             1. V + V -> V
             2. -V -> V
             3. V < V -> bool
             4. V == V -> bool
             5. V != V -> bool
             6. S * V -> V
             7. V / V -> S (explicitly convertible)
  */
  template<typename Scalar, typename Variable>
  constexpr bool is_continuous(const Variable& variable);

  //! A struct wrapper over the continuous type trait to allow for partial
  //! specializations.
  /*!
    \tparam S The type of the scalar.
    \tparam V The type of the variable.
  */
  template<typename S, typename V>
  struct ContinuousChecker {

    //! Checks whether the variable is continuous for modeling purposes.
    /*!
      \param variable The variable.
    */
    static constexpr bool check(const V& variable);
  };

  //! Type trait to check whether a variable is continuous for modeling
  //! purposes for a given scalar type.
  /*!
    \tparam S The type of the scalar.
    \tparam V The type of the variable.
    \details The variable is considered continuous if the following conditions
             are met:
             1. V + V -> V
             2. -V -> V
             3. V < V -> bool
             4. V == V -> bool
             5. V != V -> bool
             6. S * V -> V 
             7. V / V -> S (explicitly convertible)
  */
  template<typename S, typename V, typename C = void>
  struct continuous;

  template<typename S, typename V, typename C>
  struct continuous : std::false_type {};

  template<typename S, typename V>
  struct continuous<S, V, std::enable_if_t<
    std::is_convertible_v<decltype(std::declval<V>() + std::declval<V>()),
      V> &&
    std::is_convertible_v<decltype(-std::declval<V>()), V> &&
    std::is_convertible_v<decltype(std::declval<V>() < std::declval<V>()),
      bool> &&
    std::is_convertible_v<decltype(std::declval<V>() == std::declval<V>()),
      bool> &&
    std::is_convertible_v<decltype(std::declval<V>() != std::declval<V>()),
      bool> &&
    std::is_convertible_v<decltype(std::declval<S>() * std::declval<V>()),
      V> &&
    std::is_constructible_v<S, decltype(std::declval<V>() /
      std::declval<V>())>>> : std::true_type {};

  template<typename S, typename V>
  constexpr bool ContinuousChecker<S, V>::check(const V&) {
    std::cout << "Default check" << std::endl;
    return continuous<S, V>::value;
  }

  template<typename Scalar, typename Variable>
  constexpr bool is_continuous(const Variable& variable) {
    return ContinuousChecker<Scalar, Variable>::check(variable);
  }
}

#endif
