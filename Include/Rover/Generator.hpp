#ifndef ROVER_GENERATOR_HPP
#define ROVER_GENERATOR_HPP
#include <tuple>
#include "Concept.hpp"

namespace Rover {

  /** Argument generator concept */
  class Generator : Concept<Generator> {
  public:

    /** The type of generated values */
    class Type;

    //! Checks for a value in the session, and, if not found, generates 
    //! a random value
    /*!
      \param session Tuple of handles of all sub- and supergenerators and
      their respective values
    */
    template<typename Session>
    constexpr Type generate(Session& session) const;

    //! Builds a session comprising handles to all sub-generators
    auto build_session() const;
  };

  //! Produces an argument from a generator.
  /*
    \param generator The generator to evaluate.
    \return The argument produced by the <i>generator</i>.
  */
  template<typename Generator>
  auto generate(Generator& generator) {
    auto session = generator.build_session();
    return generator.generate(session);
  }

  //! Builds a session from a generator
  /*
    \param generator The generator to build a session for
    \return Session that lists all sub-generators of a generator and
    placeholders for their respective values
  */
  template<typename T, std::enable_if_t<ImplementsConcept<T,
    Generator>::value>* = nullptr>
  auto build_session(T& generator) {
    return generator.build_session();
  }

  //! Returns an empty session from a non-Generator type
  /*
    \param value Value of any non-Generator type
    \return Empty tuple
  */
  template<typename T, std::enable_if_t<!ImplementsConcept<T,
    Generator>::value>* = nullptr>
  auto build_session(T& value) {
    return std::make_tuple();
  }
}

#endif
