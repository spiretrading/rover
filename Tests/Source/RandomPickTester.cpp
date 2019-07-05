#include <catch2/catch.hpp>
#include "Rover/Constant.hpp"
#include "Rover/Generator.hpp"
#include "Rover/RandomPick.hpp"

using namespace Rover;

TEST_CASE("test_random_pick_deterministic", "[RandomPick]") {
  SECTION("Single generator.") {
    auto constant = Constant(5);
    auto random_pick = RandomPick(constant);
    REQUIRE(generate(random_pick) == 5);
  }
  SECTION("Multiple generators same value.") {
    auto c1 = Constant(5);
    auto c2 = Constant(5);
    auto c3 = Constant(5);
    auto random_pick = RandomPick(c1, c2, c3);
    REQUIRE(generate(random_pick) == 5);
  }
}

TEST_CASE("test_random_pick_arbitrary", "[RandomPick]") {
  auto c1 = Constant(3);
  auto c2 = Constant(5);
  auto c3 = Constant(7);
  auto random_pick = RandomPick(c1, c2, c3);
  for(int i = 0; i < 100; ++i) {
    auto value = generate(random_pick);
    REQUIRE((value == 3 || value == 5 || value == 7));
  }
}
