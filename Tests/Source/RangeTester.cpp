#include <string>
#include <catch.hpp>
#include "Rover/Constant.hpp"
#include "Rover/Generator.hpp"
#include "Rover/Range.hpp"

using namespace Rover;

TEST_CASE("test_int_range", "[Range]") {
  SECTION("Closed interval.") {
    for(int i = 0; i < 100; ++i) {
      auto range = Range(Constant(1), Constant(10));
      auto result = generate(range);
      REQUIRE(result >= 1);
      REQUIRE(result <= 10);
    }
  }

  SECTION("Open interval.") {
    for(int i = 0; i < 100; ++i) {
      auto range = Range(Constant(1), Constant(10), Interval::OPEN);
      auto result = generate(range);
      REQUIRE(result >= 2);
      REQUIRE(result <= 9);
    }
  }

  SECTION("Exclude left.") {
    for(int i = 0; i < 100; ++i) {
      auto range = Range(Constant(1), Constant(10), Interval::LEFT_EXCLUSIVE);
      auto result = generate(range);
      REQUIRE(result >= 2);
      REQUIRE(result <= 10);
    }
  }

  SECTION("Exclude right.") {
    for(int i = 0; i < 100; ++i) {
      auto range = Range(Constant(1), Constant(10), Interval::RIGHT_EXCLUSIVE);
      auto result = generate(range);
      REQUIRE(result >= 1);
      REQUIRE(result <= 9);
    }
  }
}

TEST_CASE("test_float_range", "[Range]") {
  for(int i = 0; i < 100; ++i) {
    auto range = Range(Constant(1.), Constant(10.));
    auto result = generate(range);
    REQUIRE(result >= 1.);
    REQUIRE(result <= 10.);
  }
}

TEST_CASE("test_mixed_types", "[Range]") {
  for(int i = 0; i < 100; ++i) {
    auto range = Range(Constant(1), Constant(10.));
    auto result = generate(range);
    static_assert(std::is_same_v<std::decay_t<decltype(result)>, double>);
    REQUIRE(result >= 1.);
    REQUIRE(result <= 10.);
  }
}

