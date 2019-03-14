#ifndef ROVER_GENERATOR_HPP
#define ROVER_GENERATOR_HPP
#include <type_traits>
#include "Rover/Evaluator.hpp"

namespace Rover {

  /** Interface for generating arguments. */
  template<typename T>
  class Generator {
    public:

      /** The type of generated values. */
      using Type = T;

      //! Generates a value.
      /*!
        \param evaluator Stores the state of the currently generated value.
      */
      Type generate(Evaluator& evaluator);
  };

  //! Produces an argument from a generator.
  /*
    \param generator The generator to evaluate.
    \return The argument produced by the <i>generator</i>.
  */
  template<typename Generator>
  auto generate(Generator& generator) {
    auto evaluator = Evaluator();
    return evaluator.evaluate(generator);
  }

  template<typename T, typename = void>
  struct is_generator : std::false_type {};

  template<typename T>
  struct is_generator<T, std::enable_if_t<!std::is_same_v<
    decltype(std::declval<T>().generate(std::declval<Evaluator&>())), void>>> :
    std::true_type {};
  
  template<typename T>
  inline constexpr bool is_generator_v = is_generator<T>::value;
}

#endif
