#include <catch.hpp>
#include "Rover/Constant.hpp"
#include "Rover/Generator.hpp"
#include "Rover/Pick.hpp"

using namespace Rover;

TEST_CASE("test_pick_fixed_index", "[Pick]") {
  SECTION("Single generator.") {
    auto constant = Constant(5);
    auto pick = Pick([] {
      return std::size_t(0);
    }, constant);
    REQUIRE(generate(pick) == 5);
  }
  SECTION("Multiple generators.") {
    auto c1 = Constant(3);
    auto c2 = Constant(5);
    auto c3 = Constant(7);
    auto pick = Pick([] {
      return std::size_t(1);
    }, c1, c2, c3);
    REQUIRE(generate(pick) == 5);
  }
}

TEST_CASE("test_pick_variable_index", "[Pick]") {
  auto c1 = Constant(3);
  auto c2 = Constant(5);
  auto c3 = Constant(7);
  auto i = std::size_t(0);
  auto pick = Pick([&] {
    auto index = i % 3;
    ++i;
    return index;
  }, c1, c2, c3);
  REQUIRE(generate(pick) == 3);
  REQUIRE(generate(pick) == 5);
  REQUIRE(generate(pick) == 7);
  REQUIRE(generate(pick) == 3);
  REQUIRE(generate(pick) == 5);
  REQUIRE(generate(pick) == 7);
  REQUIRE(generate(pick) == 3);
}
