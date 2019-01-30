#ifndef ROVER_REFERENCE_HPP
#define ROVER_REFERENCE_HPP
#include <type_traits>

namespace Rover {

  //! Used to reference an existing generator, evaluating to whatever argument
  //! its referrent has generated.
  /*!
    \tparam T The type of generator to reference.
  */
  template<typename T>
  class Reference {
    public:

      //! The type of generator to reference.
      using Generator = T;

      using Type = typename Generator::Type;

      template<typename U>
      Reference(U&& generator);

      template<typename Session>
      Type generate(Session& s);

      auto build_session() const;
  };

  template<typename U>
  Reference(U&&) -> Reference<std::decay_t<U>>;
}

#endif
