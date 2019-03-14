#include <cmath>
#include <type_traits>
#include <catch.hpp>
#include "Rover/Constant.hpp"
#include "Rover/Generator.hpp"
#include "Rover/Noncopyable.hpp"
#include "Rover/Range.hpp"

using namespace Rover;

namespace {
  template<typename T>
  class MoveOnlyConstant : public Constant<T>, private Noncopyable {
    public:
      using Constant<T>::Constant;
  };

  template<typename T>
  MoveOnlyConstant(T) -> MoveOnlyConstant<T>;
}

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

TEST_CASE("test_granularity_copyable", "[Range]") {
  SECTION("Integer.") {
    auto range = Range(Constant(1), Constant(100), Constant(5));
    for(auto i = 0; i < 100; ++i) {
      auto result = generate(range);
      REQUIRE(result >= 1);
      REQUIRE(result <= 100);
      REQUIRE(result % 5 == 0);
    }
  }
  SECTION("Double.") {
    auto range = Range(Constant(0.), Constant(1.), Constant(.1));
    for(auto i = 0; i < 100; ++i) {
      auto result = generate(range);
      REQUIRE(result >= 0.);
      REQUIRE(result <= 1.);
      REQUIRE(std::abs(result * 10. - std::round(result * 10.)) < .000001);
    }
  }
}

TEST_CASE("test_granularity_move_only", "[Range]") {
  SECTION("Integer.") {
    auto range = Range(MoveOnlyConstant(1), MoveOnlyConstant(100),
      MoveOnlyConstant(5));
    for(auto i = 0; i < 100; ++i) {
      auto result = generate(range);
      REQUIRE(result >= 1);
      REQUIRE(result <= 100);
      REQUIRE(result % 5 == 0);
    }
  }
  SECTION("Double.") {
    auto range = Range(MoveOnlyConstant(0.), MoveOnlyConstant(1.),
      MoveOnlyConstant(.1));
    for(auto i = 0; i < 100; ++i) {
      auto result = generate(range);
      REQUIRE(result >= 0.);
      REQUIRE(result <= 1.);
      REQUIRE(std::abs(result * 10. - std::round(result * 10.)) < .000001);
    }
  }
}

TEST_CASE("test_pointer_arguments", "[Range]") {
  SECTION("AllPointers.") {
    auto begin = std::make_unique<Constant<int>>(1);
    auto end = Constant<int>(100);
    auto granularity = std::make_shared<Constant<int>>(5);
    auto range = Range(std::move(begin), &end, granularity);
    for(auto i = 0; i < 100; ++i) {
      auto result = generate(range);
      REQUIRE(result >= 1);
      REQUIRE(result <= 100);
      REQUIRE(result % 5 == 0);
    }
  }
  SECTION("Mix.") {
    auto begin = std::make_unique<Constant<int>>(1);
    auto end = Constant<int>(100);
    auto granularity = std::make_shared<Constant<int>>(5);
    auto range = Range(std::move(begin), end, granularity);
    for(auto i = 0; i < 100; ++i) {
      auto result = generate(range);
      REQUIRE(result >= 1);
      REQUIRE(result <= 100);
      REQUIRE(result % 5 == 0);
    }
  }
  SECTION("MoveOnly.") {
    auto begin = std::make_unique<MoveOnlyConstant<int>>(1);
    auto end = MoveOnlyConstant<int>(100);
    auto granularity = std::make_shared<MoveOnlyConstant<int>>(5);
    auto range = Range(std::move(begin), std::move(end), granularity);
    for(auto i = 0; i < 100; ++i) {
      auto result = generate(range);
      REQUIRE(result >= 1);
      REQUIRE(result <= 100);
      REQUIRE(result % 5 == 0);
    }
  }
}
