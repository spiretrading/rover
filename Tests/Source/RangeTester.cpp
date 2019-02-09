#include <string>
#include <type_traits>
#include <vector>
#include <catch.hpp>
#include "Rover/Constant.hpp"
#include "Rover/Generator.hpp"
#include "Rover/Range.hpp"

using namespace Rover;

class SequentialEngine {
  public:

    template<typename SeedFwd>
    explicit SequentialEngine(SeedFwd&&)
        : m_current(0) {}

    int operator ()() {
      auto ret = m_current;
      ++m_current;
      if(m_current > 100) {
        m_current = 0;
      }
      return ret;
    }
  
  private:
    int m_current;
};

class NarrowingDistribution {
  public:

    NarrowingDistribution(int begin, int end)
        : m_begin(begin),
          m_end(end) {}

    template<typename Engine>
    int operator ()(Engine& engine) const {
      int val = engine();
      return val % (m_end + 1 - m_begin) + m_begin;
    }

  private:
    int m_begin;
    int m_end;
};

template<typename T>
class MiddlePeakDistribution {
  
  public:

    MiddlePeakDistribution(T begin, T end)
        : m_begin(begin),
          m_end(end) {}

    template<typename Engine>
    T operator ()(Engine&) const {
      return (m_begin + m_end) / 2;
    }

  private:
    T m_begin;
    T m_end;
};

TEST_CASE("test_int_range", "[Range]") {
  SECTION("Closed interval.") {
    auto range = Range<Reference<Constant<int>>, Reference<Constant<int>>,
        NarrowingDistribution, SequentialEngine>(Constant(5), Constant(10));
    for(int i = 0; i < 3; ++i) {
      REQUIRE(generate(range) == 5);
      REQUIRE(generate(range) == 6);
      REQUIRE(generate(range) == 7);
      REQUIRE(generate(range) == 8);
      REQUIRE(generate(range) == 9);
      REQUIRE(generate(range) == 10);
    }
  }

  SECTION("Open interval.") {
    auto range = Range<Reference<Constant<int>>, Reference<Constant<int>>,
        NarrowingDistribution, SequentialEngine>(Constant(5), Constant(10),
        Interval::OPEN);
    for(int i = 0; i < 3; ++i) {
      REQUIRE(generate(range) == 6);
      REQUIRE(generate(range) == 7);
      REQUIRE(generate(range) == 8);
      REQUIRE(generate(range) == 9);
    }
  }

  SECTION("Exclude left.") {
    auto range = Range<Reference<Constant<int>>, Reference<Constant<int>>,
        NarrowingDistribution, SequentialEngine>(Constant(5), Constant(10),
        Interval::LEFT_EXCLUSIVE);
    for(int i = 0; i < 3; ++i) {
      REQUIRE(generate(range) == 6);
      REQUIRE(generate(range) == 7);
      REQUIRE(generate(range) == 8);
      REQUIRE(generate(range) == 9);
      REQUIRE(generate(range) == 10);
    }
  }

  SECTION("Exclude right.") {
    auto range = Range<Reference<Constant<int>>, Reference<Constant<int>>,
        NarrowingDistribution, SequentialEngine>(Constant(5), Constant(10),
        Interval::RIGHT_EXCLUSIVE);
    for(int i = 0; i < 3; ++i) {
      REQUIRE(generate(range) == 5);
      REQUIRE(generate(range) == 6);
      REQUIRE(generate(range) == 7);
      REQUIRE(generate(range) == 8);
      REQUIRE(generate(range) == 9);
    }
  }
}

TEST_CASE("test_float_range", "[Range]") {
  {
    auto range = Range<Reference<Constant<float>>, Reference<Constant<float>>,
      MiddlePeakDistribution<float>>(Constant(5.f), Constant(20.f));
    REQUIRE(generate(range) == (5.f + 20.f) / 2);
    REQUIRE(generate(range) == (5.f + 20.f) / 2);
  }
  {
    auto range = Range<Reference<Constant<float>>, Reference<Constant<float>>,
      MiddlePeakDistribution<float>>(Constant(50.f), Constant(100.f));
    REQUIRE(generate(range) == (50.f + 100.f) / 2);
    REQUIRE(generate(range) == (50.f + 100.f) / 2);
  }
  {
    auto range = Range<Reference<Constant<float>>, Reference<Constant<float>>,
      MiddlePeakDistribution<float>>(Constant(5.f), Constant(10.f));
    REQUIRE(generate(range) == (5.f + 10.f) / 2);
    REQUIRE(generate(range) == (5.f + 10.f) / 2);
  }
}

TEST_CASE("test_mixed_types", "[Range]") {
  auto range = Range<Reference<Constant<int>>, Reference<Constant<double>>,
    MiddlePeakDistribution<double>>(Constant(1), Constant(9.5));
  REQUIRE(generate(range) == (static_cast<double>(1) + 9.5) / 2);
}
