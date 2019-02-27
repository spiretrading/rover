#include <memory>
#include <catch.hpp>
#include "Rover/Box.hpp"
#include "Rover/Constant.hpp"
#include "Rover/Generator.hpp"
#include "Rover/Range.hpp"

using namespace Rover;
using namespace std::string_literals;

TEST_CASE("test_values", "[Box]") {
  SECTION("Constant.") {
    auto c = Constant(5);
    auto b = Box(c);
    static_assert(std::is_same_v<std::decay_t<decltype(b)>, Box<int>>);
    REQUIRE(generate(b) == 5);
  }
  SECTION("Range.") {
    auto r = Range(Constant(5), Constant(10));
    auto b = Box(r);
    static_assert(std::is_same_v<std::decay_t<decltype(b)>, Box<int>>);
    for(auto i = 0; i < 100; ++i) {
      auto result = generate(b);
      REQUIRE(result >= 5);
      REQUIRE(result <= 10);
    }
  }
}

TEST_CASE("test_raw_pointers", "[Box]") {
  SECTION("Constant.") {
    auto c = Constant<int>(5);
    auto b = Box(&c);
    static_assert(std::is_same_v<std::decay_t<decltype(b)>, Box<int>>);
    REQUIRE(generate(b) == 5);
  }
  SECTION("Range.") {
    auto r = std::make_unique<Range<Constant<int>, Constant<int>, void>>(
      Constant(5), Constant(10));
    auto b = Box(r.get());
    static_assert(std::is_same_v<std::decay_t<decltype(b)>, Box<int>>);
    for(auto i = 0; i < 100; ++i) {
      auto result = generate(b);
      REQUIRE(result >= 5);
      REQUIRE(result <= 10);
    }
  }
}

TEST_CASE("test_unique_pointers", "[Box]") {
  SECTION("Constant.") {
    auto c = Constant<int>(5);
    auto b = Box(&c);
    static_assert(std::is_same_v<std::decay_t<decltype(b)>, Box<int>>);
    REQUIRE(generate(b) == 5);
  }
  SECTION("Range.") {
    auto r = std::make_unique<Range<Constant<int>, Constant<int>, void>>(5,
      10);
    auto b = Box(std::move(r));
    static_assert(std::is_same_v<std::decay_t<decltype(b)>, Box<int>>);
    for(auto i = 0; i < 100; ++i) {
      auto result = generate(b);
      REQUIRE(result >= 5);
      REQUIRE(result <= 10);
    }
  }
}

TEST_CASE("test_shared_pointers", "[Box]") {
  SECTION("Constant.") {
    auto c = std::make_shared<Constant<int>>(5);
    auto b = Box(c);
    static_assert(std::is_same_v<std::decay_t<decltype(b)>, Box<int>>);
    REQUIRE(generate(b) == 5);
  }
  SECTION("Range.") {
    auto r = std::make_shared<Range<Constant<int>, Constant<int>, void>>(5,
      10);
    auto b = Box(r);
    static_assert(std::is_same_v<std::decay_t<decltype(b)>, Box<int>>);
    for(auto i = 0; i < 100; ++i) {
      auto result = generate(b);
      REQUIRE(result >= 5);
      REQUIRE(result <= 10);
    }
  }
}
