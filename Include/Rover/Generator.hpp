#ifndef ROVER_GENERATOR_HPP
#define ROVER_GENERATOR_HPP

namespace Rover {

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
}

#endif
