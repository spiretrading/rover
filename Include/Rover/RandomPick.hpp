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
      class Choice {
        public:
          explicit Choice(std::size_t num_generators);
          std::size_t operator ()();

        private:
          Range<std::size_t, std::size_t> m_range;
      };
      using PickType = Pick<Choice, G...>;

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

  template<typename... G>
  RandomPick<G...>::Choice::Choice(std::size_t num_generators)
    : m_range(0, num_generators - 1) {}

  template<typename... G>
  std::size_t RandomPick<G...>::Choice::operator ()() {
    auto evaluator = Evaluator();
    return m_range.generate(evaluator);
  }

  template<typename... G>
  template<typename... GeneratorsFwd>
  RandomPick<G...>::RandomPick(GeneratorsFwd&&... generators)
    : m_pick(Choice(sizeof...(generators)), std::forward<GeneratorsFwd>(
        generators)...) {}

  template<typename... G>
  typename RandomPick<G...>::Type RandomPick<G...>::generate(Evaluator&
      evaluator) {
    return m_pick.generate(evaluator);
  }
}

#endif
