#ifndef ROVER_FILTER_HPP
#define ROVER_FILTER_HPP
#include <type_traits>
#include "Rover/Autobox.hpp"
#include "Rover/Evaluator.hpp"
#include "Rover/Generator.hpp"

namespace Rover {

  //! Applies a filter to a generator by re-evaluating the generator when the
  //! filter fails.
  /*!
    \tparam P The type of the predicate function.
    \tparam G The type of the generator.
  */
  template<typename P, typename G>
  class Filter {
    public:

      //! The type of the generator.
      using Generator = autobox_t<G>;

      //! The type of the predicate function.
      using Predicate = P;

      //! The type of generated values.
      using Type = typename Generator::Type;

      //! Constructs a Filter using a predicate function and a generator.
      /*!
        \param predicate The predicate function. The filter considered passed
                         when the function returns true.
        \param generator The generator.
      */
      template<typename PredicateFwd, typename GeneratorFwd>
      Filter(PredicateFwd&& predicate, GeneratorFwd&& generator);

      //! Evaluates the filter.
      Type generate(Evaluator& evaluator);

    private:
      Predicate m_predicate;
      Generator m_generator;
  };

  template<typename PredicateFwd, typename GeneratorFwd>
  Filter(PredicateFwd&&, GeneratorFwd&&) -> Filter<std::decay_t<PredicateFwd>,
    std::decay_t<GeneratorFwd>>;

  template<typename P, typename G>
  template<typename PredicateFwd, typename GeneratorFwd>
  Filter<P, G>::Filter(PredicateFwd&& predicate, GeneratorFwd&& generator)
    : m_predicate(std::forward<PredicateFwd>(predicate)),
      m_generator(std::forward<GeneratorFwd>(generator)) {}

  template<typename P, typename G>
  typename Filter<P, G>::Type Filter<P, G>::generate(Evaluator& evaluator) {
    while(true) {
      auto value = Rover::generate(m_generator);
      if(m_predicate(value)) {
        return value;
      }
    }
  }
}

#endif
