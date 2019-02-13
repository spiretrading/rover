#ifndef ROVER_GENERATOR_HPP
#define ROVER_GENERATOR_HPP
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
}

#endif
