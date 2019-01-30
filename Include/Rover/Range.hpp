#ifndef ROVER_RANGE_HPP
#define ROVER_RANGE_HPP
#include <random>
#include <type_traits>

namespace Rover {

  /** Type trait used to determine what default distribution to use for a given
      type.. */
  template<typename T>
  struct default_distribution_type {};

  template<typename T>
  using default_distribution_t = typename default_distribution_type<T>::type;

  /** Type trait used to determine what default random engine to use for a given
      type. */
  template<typename T>
  struct default_engine_type {};

  template<typename T>
  using default_engine_t = typename default_engine_type<T>::type;

  /** The type of interval, ie. open/closed interval. By default an interval is
      closed. */
  enum class Interval {

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
    \tparam B The type of generator representing the beginning of the range.
    \tparam E The type of generator representing the end of the range.
    \tparam D The type of distribution to produce.
    \tparam E The type of random engine to use.
  */
  template<typename B, typename E,
    typename D = default_distribution_t<
      std::common_type_t<typename B::Type, typename E::Type>>,
    typename E = default_engine_t<
      std::common_type_t<typename B::Type, typename E::Type>>>
  class Range {
    public:

      /** The type of generator representing the beginning of the range. */
      using BeginGenerator = B;

      /** The type of generator representing the end of the range. */
      using EndGenerator = E;

      /** The type of distribution to produce. */
      using Distribution = D;

      /** The type of random engine to use. */
      using Engine = E;

      using Type = std::common_type_t<typename B::Type, typename E::Type>;

      //! Constructs a closed Range over an interval defined by its
      //! sub-generators.
      /*!
        \param begin The generating evaluating the beginning of the range.
        \param end The generator evaluating the end of the range.
      */
      template<typename BF, typename EF>
      Range(BF&& begin, EF&& end);

      //! Constructs a Range over an interval defined by its sub-generators.
      /*!
        \param begin The generating evaluating the beginning of the range.
        \param end The generator evaluating the end of the range.
        \param interval The type of interval.
      */
      template<typename BF, typename EF>
      Range(BF&& begin, EF&& end, Interval interval);

      template<typename Session>
      constexpr Type generate(Session& session) const;

      auto build_session() const;
  };

  template<typename BF, typename EF>
  Range(BF&&, EF&&) -> Range<std::decay_t<BF>, std::decay_t<EF>>;

  template<typename BF, typename EF>
  Range(BF&&, EF&&, Interval) -> Range<std::decay_t<BF>, std::decay_t<EF>>;
}

#endif
