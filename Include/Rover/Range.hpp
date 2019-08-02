#ifndef ROVER_RANGE_HPP
#define ROVER_RANGE_HPP
#include <cmath>
#include <functional>
#include <random>
#include <type_traits>
#include <utility>
#include "Rover/Autobox.hpp"
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
  template<typename B, typename E, typename G = void>
  class Range {
    public:

      //! The type of generator evaluating to the beginning of the range.
      using Begin = autobox_t<B>;

      //! The type of generator evaluating to the end of the range.
      using End = autobox_t<E>;

      //! The type used to determine the granularity of the interval.
      using Granularity = autobox_t<G>;

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

    private:
      using GranularityPlaceholder = std::conditional_t<std::is_same_v<
        Granularity, void>, char, Granularity>;
      Begin m_begin;
      End m_end;
      GranularityPlaceholder m_granularity;
      Interval m_interval;
      std::mt19937 m_engine;
      std::uniform_real_distribution<double> m_distribution;

      Type calculate_random(const Type& begin, const Type& end);
      Type pick_random(const Type& lhs, const Type& rhs);
      template<typename TypeFwd>
      Type round(Evaluator& evaluator, TypeFwd&& value);
  };

  template<typename BeginFwd, typename EndFwd>
  Range(BeginFwd&&, EndFwd&&, Interval = Interval::CLOSED) ->
    Range<std::decay_t<BeginFwd>, std::decay_t<EndFwd>, void>;

  template<typename BeginFwd, typename EndFwd, typename GranularityFwd>
  Range(BeginFwd&&, EndFwd&&, GranularityFwd&&, Interval = Interval::CLOSED) ->
    Range<std::decay_t<BeginFwd>, std::decay_t<EndFwd>,
    std::decay_t<GranularityFwd>>;

  template<typename B, typename E, typename G>
  template<typename BeginFwd, typename EndFwd>
  Range<B, E, G>::Range(BeginFwd&& begin, EndFwd&& end,
      Interval interval)
      : m_begin(std::forward<BeginFwd>(begin)),
        m_end(std::forward<EndFwd>(end)),
        m_interval(interval),
        m_engine(std::random_device()()),
        m_distribution(0., 1.) {}

  template<typename B, typename E, typename G>
  template<typename BeginFwd, typename EndFwd, typename GranularityFwd>
  Range<B, E, G>::Range(BeginFwd&& begin, EndFwd&& end,
      GranularityFwd&& granularity, Interval interval)
      : m_begin(std::forward<BeginFwd>(begin)),
        m_end(std::forward<EndFwd>(end)),
        m_granularity(std::forward<GranularityFwd>(granularity)),
        m_interval(interval),
        m_engine(std::random_device()()),
        m_distribution(0., 1.) {}

  template<typename B, typename E, typename G>
  typename Range<B, E, G>::Type Range<B, E, G>::generate(
      Evaluator& evaluator) {
    auto begin = evaluator.evaluate(m_begin);
    auto end = evaluator.evaluate(m_end);
    if(begin == end) {
      return begin;
    }
    while(true) {
      auto value = calculate_random(begin, end + end);
      auto alt_value = calculate_random(begin, end + end);
      if(value == begin && alt_value == begin) {
        continue;
      } else if(alt_value == begin) {
        value = end;
      } else if(value >= end) {
        continue;
      }
      auto result = round(evaluator, std::move(value));
      switch(m_interval) {
        case Interval::CLOSED:
          if(result >= begin && result <= end) {
            return result;
          }
          break;
        case Interval::LEFT_EXCLUSIVE:
          if(result > begin && result <= end) {
            return result;
          }
          break;
        case Interval::RIGHT_EXCLUSIVE:
          if(result >= begin && result < end) {
            return result;
          }
          break;
        case Interval::OPEN:
          if(result > begin && result < end) {
            return result;
          }
          break;
      }
    }
  }

  template<typename B, typename E, typename G>
  typename Range<B, E, G>::Type Range<B, E, G>::calculate_random(
      const Type& begin, const Type& end) {
    auto random_fraction = m_distribution(m_engine);
    auto distance = static_cast<Type>(random_fraction * (end - begin));
    auto value = begin + distance;
    return value;
  }

  template<typename B, typename E, typename G>
  typename Range<B, E, G>::Type Range<B, E, G>::pick_random(
      const Type& lhs, const Type& rhs) {
    auto random_fraction = m_distribution(m_engine);
    if(random_fraction < 0.5) {
      return lhs;
    } else {
      return rhs;
    }
  }

  template<typename B, typename E, typename G>
  template<typename TypeFwd>
  typename Range<B, E, G>::Type Range<B, E, G>::round(
      Evaluator& evaluator, TypeFwd&& value) {
    using namespace std;
    if constexpr(std::is_same_v<G, void>) {
      return std::forward<TypeFwd>(value);
    } else {
      auto granularity = evaluator.evaluate(m_granularity);
      auto flr = static_cast<Type>(granularity * floor(value / granularity));
      auto ceil = static_cast<Type>(granularity * (floor(value / granularity) +
        1));
      if(abs(value - flr) <= abs(ceil - value)) {
        return flr;
      } else {
        return ceil;
      }
    }
  }
}

#endif
