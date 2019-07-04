#include <string>
#include <catch2/catch.hpp>
#include "Rover/Autobox.hpp"
#include "Rover/Box.hpp"
#include "Rover/Constant.hpp"
#include "Rover/Generator.hpp"

using namespace Rover;
using namespace std::string_literals;

TEST_CASE("test_constants", "[Autobox]") {
  SECTION("Value.") {
    auto gen = make_autobox(5);
    REQUIRE(generate(gen) == 5);
  }
  SECTION("Pointer.") {
    auto val = 5;
    auto gen = make_autobox(&val);
    REQUIRE(generate(gen) == &val);
  }
}

TEST_CASE("test_generators_by_value", "[Autobox]") {
  SECTION("Constant.") {
    auto gen = make_autobox(Constant(5));
    static_assert(std::is_same_v<std::decay_t<decltype(gen)>, Constant<int>>);
    REQUIRE(generate(gen) == 5);
  }
  SECTION("Box.") {
    auto box = make_autobox(std::make_unique<Constant<int>>(5));
    auto gen = make_autobox(std::move(box));
    static_assert(std::is_same_v<std::decay_t<decltype(gen)>, Box<int>>);
    REQUIRE(generate(gen) == 5);
  }
}

TEST_CASE("test_generators_by_pointer", "[Autobox]") {
  SECTION("RawPointer.") {
    auto c = Constant(5);
    auto gen = make_autobox(&c);
    static_assert(std::is_same_v<std::decay_t<decltype(gen)>, Box<int>>);
    REQUIRE(generate(gen) == 5);
  }
  SECTION("UniquePointer.") {
    auto gen = make_autobox(std::make_unique<Constant<int>>(5));
    static_assert(std::is_same_v<std::decay_t<decltype(gen)>, Box<int>>);
    REQUIRE(generate(gen) == 5);
  }
  SECTION("SharedPointer.") {
    auto gen = make_autobox(std::make_shared<Constant<int>>(5));
    static_assert(std::is_same_v<std::decay_t<decltype(gen)>, Box<int>>);
    REQUIRE(generate(gen) == 5);
  }
}
