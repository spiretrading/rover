#include <optional>
#include <string>
#include <catch.hpp>
#include "Rover/Constant.hpp"
#include "Rover/Generator.hpp"
#include "Rover/Range.hpp"
#include "Rover/Reference.hpp"

using namespace Rover;
using namespace std::string_literals;

TEST_CASE("test_independent", "[Reference]") {
  SECTION("Constant.") {
    auto ref = Reference(Constant(5));
    REQUIRE(generate(ref) == 5);
  }
  SECTION("Range.") {
    for(int i = 0; i < 100; ++i) {
      auto ref = Reference(Range(Constant(5), Constant(10)));
      auto result = generate(ref);
      REQUIRE(generate(ref) >= 5);
      REQUIRE(generate(ref) <= 10);
    }
  }
}

TEST_CASE("test_dangling", "[Reference]") {
  SECTION("Destroy original generator") {
    std::optional<Reference<Constant<int>>> ref;
    {
      auto c = Constant(5);
      ref.emplace(std::move(c));
    }
    REQUIRE(generate(*ref) == 5);
  }
  SECTION("Destroy original reference") {
    std::optional<Reference<Constant<int>>> ref;
    {
      auto r = Reference(Constant(5));
      ref.emplace(r);
    }
    REQUIRE(generate(*ref) == 5);
  }
  SECTION("Destroy dependent reference") {
    auto ref = Reference(Constant(5));
    {
      auto r = ref;
    }
    REQUIRE(generate(ref) == 5);
  }
}

TEST_CASE("test_make_reference", "[Reference]") {
  auto r1 = make_reference<Constant>(5);
  auto r2 = make_reference<Range>(r1, make_reference<Constant>(10));
  auto r3 = make_reference<Range>(r1, r2);
  auto result = generate(r3);
  REQUIRE(result >= 5);
  REQUIRE(result <= 10);
}
