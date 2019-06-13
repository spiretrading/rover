#ifndef ROVER_RANDOM_PICK_HPP
#define ROVER_RANDOM_PICK_HPP
#include "Rover/Evaluator.hpp"
#include "Rover/Pick.hpp"
#include "Rover/Range.hpp"

namespace Rover {

  //! An argument generator that evaluates to one of the arguments with an
  //! equal likelyhood.
  /*!
    \tparam G The types of the generators.
  */
  template<typename... G>
  class RandomPick {
    private:
      using PickType = Pick<Range<int, int>, G...>;

    public:

      //! The type of generated values.
      using Type = typename PickType::Type;

      //! Constructs RandomPick.
      /*!
        \param generators One or more generators evaluating to the same type.
      */
      template<typename... GeneratorsFwd>
      explicit RandomPick(GeneratorsFwd&&... generators);

      //! Evaluates the generator.
      Type generate(Evaluator& evaluator);

    private:
      PickType m_pick;
  };

  template<typename... GeneratorsFwd>
  RandomPick(GeneratorsFwd&&...) -> RandomPick<std::decay_t<GeneratorsFwd>...>;

  //! GCC workaround.
  template<typename GeneratorFwd, typename... GeneratorsFwd>
  RandomPick(GeneratorFwd&&, GeneratorsFwd&&...) -> RandomPick<std::decay_t<
    GeneratorFwd>, std::decay_t<GeneratorsFwd>...>;

  template<typename... G>
  template<typename... GeneratorsFwd>
  RandomPick<G...>::RandomPick(GeneratorsFwd&&... generators)
    : m_pick(Range(0, static_cast<int>(sizeof...(generators) - 1)),
        std::forward<GeneratorsFwd>(generators)...) {}

  template<typename... G>
  typename RandomPick<G...>::Type RandomPick<G...>::generate(Evaluator&
      evaluator) {
    return m_pick.generate(evaluator);
  }
}

#endif
