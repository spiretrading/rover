#ifndef ROVER_RANGE_HPP
#define ROVER_RANGE_HPP
#include <functional>
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

    private:
      Type calculate_random(const Type& begin, const Type& end);
      Type pick_random(const Type& lhs, const Type& rhs);

    private:
      Begin m_begin;
      End m_end;
      std::function<Type(const Type&)> m_round;
      Interval m_interval;
      std::mt19937 m_engine;
      std::uniform_real_distribution<double> m_distribution;
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
        m_round([](const Type& value){
          return value;
        }),
        m_interval(interval),
        m_engine(std::random_device()()),
        m_distribution(0., 1.) {}

  template<typename B, typename E, typename G>
  template<typename BeginFwd, typename EndFwd, typename GranularityFwd>
  Range<B, E, G>::Range(BeginFwd&& begin, EndFwd&& end,
      GranularityFwd&& granularity, Interval interval)
      : m_begin(std::forward<BeginFwd>(begin)),
        m_end(std::forward<EndFwd>(end)),
        m_round([granularity = std::forward<GranularityFwd(granularity)](
            const Type& value){
          auto flr = granularity * static_cast<int64_t>(value / granularity);
          auto ceil = granularity * (static_cast<int64_t>(value /
              granularity) + 1);
          return std::abs(value - flr) <= std::abs(ceil - value) ?
            flr : ceil;
        }),
        m_interval(interval),
        m_engine(std::random_device()()),
        m_distribution(0., 1.) {}

  template<typename B, typename E, typename G>
  typename Range<B, E, G>::Type Range<B, E, G>::generate(
      Evaluator& evaluator) {
    auto begin = evaluator.evaluate(m_begin);
    auto end = evaluator.evaluate(m_end);
    while(true) {
      auto value = calculate_random(begin, end);
      auto alt_value = calculate_random(begin, end);
      if(value == begin && alt_value == begin) {
        continue;
      } else if(value == begin || alt_value == begin) {
        value = pick_random(begin, end);
      }
      auto result = m_round(value);
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
}

#endif
