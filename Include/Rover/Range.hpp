#ifndef ROVER_RANGE_HPP
#define ROVER_RANGE_HPP
#include <random>
#include <type_traits>
#include <utility>
#include "Rover/Evaluator.hpp"

namespace Rover {

  /** Type trait used to determine what default distribution to use for a given
      type. */
  template<typename T, typename = void>
  struct default_distribution_type {};

  template<typename T>
  struct default_distribution_type<T,
      std::enable_if_t<std::is_integral_v<T>>> {
    using type = std::uniform_int_distribution<T>;
  };

  template<typename T>
  struct default_distribution_type<T,
      std::enable_if_t<std::is_floating_point_v<T>>> {
    using type = std::uniform_real_distribution<T>;
  };

  template<typename T>
  using default_distribution_t = typename default_distribution_type<T>::type;

  /** Type trait used to determine what default random engine to use for a
      given type. */
  template<typename T>
  struct default_engine_type {
    using type = std::mt19937;
  };

  template<typename T>
  using default_engine_t = typename default_engine_type<T>::type;

  /** The type of interval, ie. open/closed interval. By default an interval is
      closed. */
  enum class Interval {

    /** A range from [a, b]. */
    CLOSED = 0,

    /** A range from (a, b]. */
    LEFT_EXCLUSIVE = 1,

    /** A range from [a, b). */
    RIGHT_EXCLUSIVE = 2,

    /** An open range (a, b). */
    OPEN = LEFT_EXCLUSIVE | RIGHT_EXCLUSIVE
  };

  //! Generates an argument within a range of values determined by
  //! sub-generators.
  /*!
    \tparam B The type of generator evaluating to the beginning of the range.
    \tparam E The type of generator evaluating to the end of the range.
    \tparam D The type of distribution to produce.
    \tparam E The type of random engine to use.
  */
  template<typename B, typename E, typename D = default_distribution_t<
    std::common_type_t<typename B::Type, typename E::Type>>,
    typename A = default_engine_t<std::common_type_t<typename B::Type,
    typename E::Type>>>
  class Range {
    public:

      //! The type of generator evaluating to the beginning of the range.
      using Begin = B;

      //! The type of generator evaluating to the end of the range.
      using End = E;

      //! The type of distribution to produce.
      using Distribution = D;

      //! The type of random engine to use.
      using Engine = A;
      using Type = std::common_type_t<typename Begin::Type, typename End::Type>;

      //! Constructs a Range over an interval defined by its sub-generators.
      /*!
        \param begin The generator evaluating to the beginning of the range.
        \param end The generator evaluating to the end of the range.
        \param interval The type of interval.
        \warning begin and end must define a valid non-empty interval. No check
                 is made if an open interval is empty, which will result in an
                 infinite loop in generate.
      */
      template<typename BeginFwd, typename EndFwd>
      Range(BeginFwd&& begin, EndFwd&& end,
        Interval interval = Interval::CLOSED);

      Type generate(Evaluator& evaluator);

    private:
      Begin m_begin;
      End m_end;
      Interval m_interval;
      Engine m_engine;
      Distribution m_distribution;
  };

  template<typename BeginFwd, typename EndFwd>
  Range(BeginFwd&&, EndFwd&&, Interval = Interval::CLOSED) ->
    Range<std::decay_t<BeginFwd>, std::decay_t<EndFwd>>;

  template<typename B, typename E, typename D, typename A>
  template<typename BeginFwd, typename EndFwd>
  Range<B, E, D, A>::Range(BeginFwd&& begin, EndFwd&& end, Interval interval)
      : m_begin(std::forward<BeginFwd>(begin)),
        m_end(std::forward<EndFwd>(end)),
        m_interval(interval),
        m_engine(std::random_device()()) {}

  template<typename B, typename E, typename D, typename A>
  typename Range<B, E, D, A>::Type Range<B, E, D, A>::generate(
      Evaluator& evaluator) {
    auto begin = evaluator.evaluate(m_begin);
    auto end = evaluator.evaluate(m_end);
    while(true) {
      auto result = m_distribution(m_engine);
      switch(m_interval) {
        case Interval::CLOSED:
          return result;
        case Interval::LEFT_EXCLUSIVE:
          if(result != begin) {
            return result;
          }
          break;
        case Interval::RIGHT_EXCLUSIVE:
          if(result != end) {
            return result;
          }
          break;
        case Interval::OPEN:
          if(result != begin && result != end) {
            return result; 
          }
          break;
      }
    }
  }
}

#endif
