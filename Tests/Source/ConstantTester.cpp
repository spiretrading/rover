#include <string>
#include <catch.hpp>
#include "Rover/Constant.hpp"
#include "Rover/Generator.hpp"
#include "Rover/Noncopyable.hpp"

using namespace Rover;
using namespace std::string_literals;

TEST_CASE("test_fundamental_constants", "[Constant]") {
  SECTION("Boolean values.") {
    auto true_generator = Constant(true);
    REQUIRE(generate(true_generator));
    auto false_generator = Constant(false);
    REQUIRE(!generate(false_generator));
  }
  SECTION("Integral values.") {
    auto char_generator = Constant('a');
    REQUIRE(generate(char_generator) == 'a');
    auto int_generator = Constant(123);
    REQUIRE(generate(int_generator) == 123);
  }
  SECTION("Floating values.") {
    auto float_generator = Constant(2.71f);
    REQUIRE(generate(float_generator) == 2.71f);
    auto double_generator = Constant(3.141);
    REQUIRE(generate(double_generator) == 3.141);
  }
}

TEST_CASE("test_class_constants", "[Constant]") {
  auto string_generator = Constant("hello"s);
  REQUIRE(generate(string_generator) == "hello"s);
}
