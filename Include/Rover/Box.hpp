#ifndef ROVER_BOX_HPP
#define ROVER_BOX_HPP
#include "Rover/Evaluator.hpp"
#include "Rover/Noncopyable.hpp"

namespace Rover {

  //! Provides type-erasure over generators evaluating to a type T.
  /*!
    \tparam T The type of value to generate.
  */
  template<typename T>
  class Box : private Noncopyable {
    public:

      //! The type of value to generate.
      using Type = T;

      Type generate(Evaluator& evaluator) const;
  };
}

#endif
