#ifndef ROVER_LIFT_HPP
#define ROVER_LIFT_HPP
#include <tuple>
#include <type_traits>
#include "Rover/Evaluator.hpp"

namespace Rover {

  /** An argument generator that evaluates a function of other generators. */
  template<typename F, typename... Generators>
  class Lift {
  public:

    /** The type of a callable to apply to sub-generator results */
    using Function = F;

    /** The return type of generate. */
    using Type = std::invoke_result_t<Function, typename Generators::Type...>;

    //! Constructs a lift from a callable and sub-generators.
    /*!
      \param func Callable that takes sub-generator results as input arguments
             and returns a value.
      \param generators Sub-generators used to produce the input arguments for
             the callable
    */
    template<typename FunctionFwd, typename... GeneratorsFwd>
    constexpr Lift(FunctionFwd&& func, GeneratorsFwd&&... generators);

    constexpr Type generate(Evaluator& evaluator);

  private:
    Function m_func;
    std::tuple<Generators...> m_generators;
  };

  template<typename FunctionFwd, typename... GeneratorsFwd>
  Lift(FunctionFwd&&, GeneratorsFwd&&...) -> Lift<std::decay_t<FunctionFwd>,
      std::decay_t<GeneratorsFwd>...>;

  template<typename F, typename... Generators>
  template<typename FunctionFwd, typename... GeneratorsFwd>
  constexpr Lift<F, Generators...>::Lift(FunctionFwd&& func,
      GeneratorsFwd&&... generators)
      : m_func(std::forward<FunctionFwd>(func)),
        m_generators(std::forward<GeneratorsFwd>(generators)...) {}

  template<typename F, typename... Generators>
  constexpr typename Lift<F, Generators...>::Type
      Lift<F, Generators...>::generate(Evaluator& evaluator) {
    return std::apply([this, &evaluator](auto&... generators) {
      return m_func(evaluator.evaluate(generators)...);
    }, m_generators);
  }
}

#endif
