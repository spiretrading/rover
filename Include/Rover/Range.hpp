#ifndef ROVER_RANGE_HPP
#define ROVER_RANGE_HPP
#include <random>
#include <stdexcept>
#include <type_traits>
#include "Noncopyable.hpp"
#include "Reference.hpp"

namespace Rover {

  namespace Details {

    template<typename Begin, typename End>
    void check_range(const Begin& begin, const End& end) {
      if(end < begin) {
        throw std::out_of_range("Invalid range!");
      }
    }
  }

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
    \tparam Begin The type of reference representing the beginning of the
            range.
    \tparam End The type of reference representing the end of the range.
    \tparam Distribution The type of distribution to produce.
    \tparam Engine The type of random engine to use.
  */
  template<typename Begin, typename End,
    typename Distribution = default_distribution_t<
      std::common_type_t<typename Begin::Type, typename End::Type>>,
    typename Engine = default_engine_t<
      std::common_type_t<typename Begin::Type, typename End::Type>>>
  class Range : Noncopyable {
    static_assert(is_reference_v<Begin>,
        "The beginning of the range is not a Reference");
    static_assert(is_reference_v<End>,
        "The end of the range is not a Reference");

    public:

      using Type = std::common_type_t<typename Begin::Type,
        typename End::Type>;

      //! Constructs a Range over an interval defined by its sub-generators.
      /*!
        \param begin The generator evaluating the beginning of the range.
        \param end The generator evaluating the end of the range.
        \param interval The type of interval.
        \warning begin and end must define a valid non-empty interval. No check
                 is made if an open interval is empty, which will result in an
                 infinite loop in generate
      */
      template<typename BeginFwd, typename EndFwd>
      Range(BeginFwd&& begin, EndFwd&& end, Interval interval =
          Interval::CLOSED);

      Range(Range&& other);

      template<typename Session>
      constexpr Type generate(Session& session);

      auto build_session() const;

    private:
      Engine m_engine;
      Begin m_begin;
      End m_end;
      Interval m_interval;
  };

  template<typename BeginFwd, typename EndFwd>
  Range(BeginFwd&&, EndFwd&&, Interval = Interval::CLOSED) -> 
      Range<ensure_reference_t<std::decay_t<BeginFwd>>,
      ensure_reference_t<std::decay_t<EndFwd>>>;

  template<typename Begin, typename End, typename Distribution,
      typename Engine>
  template<typename BeginFwd, typename EndFwd>
  Range<Begin, End, Distribution, Engine>::Range
      (BeginFwd&& begin, EndFwd&& end, Interval interval)
      : m_engine(std::random_device()()),
        m_begin(std::forward<BeginFwd>(begin)),
        m_end(std::forward<EndFwd>(end)),
        m_interval(interval) {}

  template<typename Begin, typename End, typename Distribution,
      typename Engine>
  Range<Begin, End, Distribution, Engine>::Range
      (Range<Begin, End, Distribution, Engine>&& other)
      : m_begin(std::move(other.m_begin)),
        m_end(std::move(other.m_end)),
        m_interval(std::move(other.m_interval)) {}

  template<typename Begin, typename End, typename Distribution,
      typename Engine>
  template<typename Session>
  constexpr typename Range<Begin, End, Distribution, Engine>::Type
      Range<Begin, End, Distribution, Engine>::generate(Session& session) {
    while(true) {
      auto begin = m_begin.generate(session);
      auto end = m_end.generate(session);
      Details::check_range(begin, end);
      auto distribution = Distribution(begin, end);
      auto result = distribution(m_engine);
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

  template<typename Begin, typename End, typename Distribution,
      typename Engine>
  auto Range<Begin, End, Distribution, Engine>::build_session() const {
    return std::tuple_cat(m_begin.build_session(), m_end.build_session());
  }

  template<typename Begin, typename End, typename Distribution,
      typename Engine>
  struct ImplementsConcept<Range<Begin, End, Distribution, Engine>,
      Generator> : std::true_type {};
}

#endif
