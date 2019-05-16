#ifndef ROVER_PICK_HPP
#define ROVER_PICK_HPP
#include <tuple>
#include <type_traits>
#include "Rover/Evaluator.hpp"

namespace Rover {

  //! An argument generator that evaluates to one of the arguments based on
  //! the result of a choice generator.
  /*!
    \tparam C The type of the choice generator.
    \tparam G The types of the generators.
  */
  template<typename C, typename... G>
  class Pick {
    public:

      //! The type of the choice generator.
      using Choice = C;

      //! The types of the generators.
      using Generators = std::tuple<G...>;

      //! The type of generated values.
      using Type = std::common_type_t<typename G::Type...>;

      //! Constructs Pick.
      /*!
        \param choice Function returning the index of the next generator to
                      evaluate.
        \param generator Mandatory first generator.
        \param generators Optional additional generators evaluating to the same
                          type.
      */
      template<typename ChoiceFwd, typename GeneratorFwd, typename...
        GeneratorsFwd>
      Pick(ChoiceFwd&& choice, GeneratorFwd&& generator, GeneratorsFwd&&...
        generators);

      //! Evaluates the generator.
      Type generate(Evaluator& evaluator);

    private:
      Choice m_choice;
      Generators m_generators;
  };

  template<typename ChoiceFwd, typename GeneratorFwd, typename...
    GeneratorsFwd>
  Pick(ChoiceFwd&&, GeneratorFwd&&, GeneratorsFwd&&...) -> Pick<std::decay_t<
    ChoiceFwd>, std::decay_t<GeneratorFwd>, std::decay_t<GeneratorsFwd>...>;

  template<typename C, typename... G>
  template<typename ChoiceFwd, typename GeneratorFwd, typename...
    GeneratorsFwd>
  Pick<C, G...>::Pick(ChoiceFwd&& choice, GeneratorFwd&& generator,
      GeneratorsFwd&&... generators)
    : m_choice(std::forward<ChoiceFwd>(choice)),
      m_generators(std::forward<GeneratorFwd>(generator), std::forward<
        GeneratorsFwd>(generators)...) {}

namespace Details {
  template<typename T>
  struct ByIndexEvaluator {
    template<std::size_t I = 0>
    static auto evaluate(T& tuple, Evaluator& evaluator, std::size_t index) {
      if(I == index) {
        return evaluator.evaluate(std::get<I>(tuple));
      } else {
        return evaluate<I + 1>(tuple, evaluator, index);
      }
    }

    template<>
    static auto evaluate<std::tuple_size_v<T>>(T& tuple, Evaluator& evaluator,
        std::size_t index) {
      return evaluator.evaluate(std::get<0>(tuple));
    }
  };
}

  template<typename C, typename... G>
  typename Pick<C, G...>::Type Pick<C, G...>::generate(Evaluator& evaluator) {
    auto index = evaluator.evaluate(m_choice);
    auto value = Details::ByIndexEvaluator<Generators>::evaluate(m_generators,
      evaluator, index);
    return value;
  }
}

#endif
