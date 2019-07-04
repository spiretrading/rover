#include <catch.hpp>
#include "Rover/Constant.hpp"
#include "Rover/Filter.hpp"
#include "Rover/Generator.hpp"
#include "Rover/Lift.hpp"
#include "Rover/Pick.hpp"

using namespace Rover;

TEST_CASE("test_single_filter", "[Filter]") {
  SECTION("Passing constant.") {
    auto filter = Filter([](auto x) {
        return x == 5;
      }, 5);
    REQUIRE(generate(filter) == 5);
    REQUIRE(generate(filter) == 5);
  }
  SECTION("First to pass.") {
    auto i = 0;
    auto pick = Pick(Lift([&] {
        auto result = i % 2;
        ++i;
        return result;
      }), Constant(10), Constant(5));
    auto filter = Filter([](auto x) {
        return x > 7;
      }, std::move(pick));
    REQUIRE(generate(filter) == 10);
    REQUIRE(generate(filter) == 10);
  }
  SECTION("Second to pass.") {
    auto i = 0;
    auto pick = Pick(Lift([&] {
        auto result = i % 2;
        ++i;
        return result;
      }), Constant(10), Constant(5));
    auto filter = Filter([](auto x) {
        return x < 7;
      }, std::move(pick));
    REQUIRE(generate(filter) == 5);
    REQUIRE(generate(filter) == 5);
  }
}

TEST_CASE("test_stacked_filters", "[Filter]") {
  auto i = 0;
  auto pick = Pick(Lift([&] {
      auto result = i % 4;
      ++i;
      return result;
    }), Constant(12), Constant(6), Constant(8), Constant(9));
  auto filter1 = Filter([](auto x) {
      return x > 7;
    }, std::move(pick));
  auto filter2 = Filter([](auto x) {
      return x < 10;
    }, std::move(filter1));
  REQUIRE(generate(filter2) == 8);
  REQUIRE(generate(filter2) == 9);
  REQUIRE(generate(filter2) == 8);
  REQUIRE(generate(filter2) == 9);
}
