#ifndef ROVER_RANGE_HPP
#define ROVER_RANGE_HPP
#include <random>
#include <type_traits>
#include <utility>
#include "Rover/Evaluator.hpp"

namespace Rover {

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
    \tparam G The type of generator used to determine the granularity of the
              interval.
  */
  template<typename B, typename E, typename G>
  class Range {
    public:

      //! The type of generator evaluating to the beginning of the range.
      using Begin = B;

      //! The type of generator evaluating to the end of the range.
      using End = E;

      //! The type used to determine the granularity of the interval.
      using Granularity = G;

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

      //! Constructs a Range over an interval defined by its sub-generators.
      /*!
        \param begin The generator evaluating to the beginning of the range.
        \param end The generator evaluating to the end of the range.
        \param granularity The granularity of the range.
        \param interval The type of interval.
        \warning begin and end must define a valid non-empty interval. No check
                 is made if an open interval is empty, which will result in an
                 infinite loop in generate.
      */
      template<typename BeginFwd, typename EndFwd, typename GranularityFwd>
      Range(BeginFwd&& begin, EndFwd&& end, GranularityFwd&& granularity,
        Interval interval = Interval::CLOSED);

      Type generate(Evaluator& evaluator);
  };

  template<typename BeginFwd, typename EndFwd>
  Range(BeginFwd&&, EndFwd&&, Interval = Interval::CLOSED) ->
    Range<std::decay_t<BeginFwd>, std::decay_t<EndFwd>, void>;
}

#endif
