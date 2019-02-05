#include <iostream>
#include <string>
#include <catch.hpp>
#include "Rover/Composite.hpp"
#include "Rover/Constant.hpp"
#include "Rover/Generator.hpp"
#include "Rover/Range.hpp"
#include "Rover/Reference.hpp"
#include "Rover/Utilities.hpp"

using namespace Rover;
using namespace std::string_literals;

TEST_CASE("test_independent_constants", "[Composite]") {
  SECTION("Boolean values.") {
    auto true_generator = Constant(true);
    auto false_generator = Constant(false);
    auto composite_generator = Composite(std::move(true_generator),
      std::move(false_generator));
    auto result = generate(composite_generator);
    static_assert(std::tuple_size_v<decltype(result)> == 2);
    REQUIRE(std::get<0>(result));
    REQUIRE(!std::get<1>(result));
  }
  SECTION("Integral values.") {
    auto char_generator = Constant('a');
    auto int_generator = Constant(123);
    auto composite_generator = Composite(std::move(char_generator),
      std::move(int_generator));
    auto result = generate(composite_generator);
    static_assert(std::tuple_size_v<decltype(result)> == 2);
    REQUIRE(std::get<0>(result) == 'a');
    REQUIRE(std::get<1>(result) == 123);
  }
  SECTION("Floating values.") {
    auto float_generator = Constant(2.71f);
    auto double_generator = Constant(3.141);
    Reference r1 = std::move(float_generator);
    Reference r2 = std::move(double_generator);
    auto composite_generator = Composite(std::move(float_generator),
      std::move(double_generator));
    auto result = generate(composite_generator);
    static_assert(std::tuple_size_v<decltype(result)> == 2);
    REQUIRE(std::get<0>(result) == 2.71f);
    REQUIRE(std::get<1>(result) == 3.141);
  }
}

TEST_CASE("test_dependent_constants", "[Composite]") {
  SECTION("Serial dependency") {
    auto c1 = Reference(Constant(5));
    auto c2 = c1;
    auto c3 = c2;
    auto generator = Composite(c1, c2, c3);
    auto result = generate(generator);
    static_assert(std::tuple_size_v<decltype(result)> == 3);
    REQUIRE(std::get<0>(result) == 5);
    REQUIRE(std::get<1>(result) == 5);
    REQUIRE(std::get<2>(result) == 5);
  }
  SECTION("Parallel dependency") {
    auto c1 = Reference(Constant(5));
    auto c2 = c1;
    auto c3 = c1;
    auto generator = Composite(c1, c2, c3);
    auto result = generate(generator);
    static_assert(std::tuple_size_v<decltype(result)> == 3);
    REQUIRE(std::get<0>(result) == 5);
    REQUIRE(std::get<1>(result) == 5);
    REQUIRE(std::get<2>(result) == 5);
  }
  SECTION("Latent serial") {
    auto c1 = Reference(Constant(5));
    auto c2 = c1;
    auto c3 = c2;
    auto generator = Composite(c2, c3);
    auto result = generate(generator);
    static_assert(std::tuple_size_v<decltype(result)> == 2);
    REQUIRE(std::get<0>(result) == 5);
    REQUIRE(std::get<1>(result) == 5);
  }
  SECTION("Latent parallel") {
    auto c1 = Reference(Constant(5));
    auto c2 = c1;
    auto c3 = c1;
    auto generator = Composite(c2, c3);
    auto result = generate(generator);
    static_assert(std::tuple_size_v<decltype(result)> == 2);
    REQUIRE(std::get<0>(result) == 5);
    REQUIRE(std::get<1>(result) == 5);
  }
}

TEST_CASE("test_dependent_ranges", "[Composite]") {
  SECTION("Serial dependency") {
    auto c1 = Reference(Range(Constant(1.), Constant(10.)));
    auto c2 = c1;
    auto c3 = c2;
    auto generator = Composite(c1, c2, c3);
    auto result = generate(generator);
    static_assert(std::tuple_size_v<decltype(result)> == 3);
    REQUIRE(std::get<0>(result) == std::get<1>(result));
    REQUIRE(std::get<1>(result) == std::get<2>(result));
  }
  SECTION("Parallel dependency") {
    auto c1 = Reference(Range(Constant(1.), Constant(10.)));
    auto c2 = c1;
    auto c3 = c1;
    auto generator = Composite(c1, c2, c3);
    auto result = generate(generator);
    static_assert(std::tuple_size_v<decltype(result)> == 3);
    REQUIRE(std::get<0>(result) == std::get<1>(result));
    REQUIRE(std::get<1>(result) == std::get<2>(result));
  }
  SECTION("Latent serial") {
    auto c1 = Reference(Range(Constant(1.), Constant(10.)));
    auto c2 = c1;
    auto c3 = c2;
    auto generator = Composite(c2, c3);
    auto result = generate(generator);
    static_assert(std::tuple_size_v<decltype(result)> == 2);
    REQUIRE(std::get<0>(result) == std::get<1>(result));
  }
  SECTION("Latent parallel") {
    auto c1 = Reference(Range(Constant(1.), Constant(10.)));
    auto c2 = c1;
    auto c3 = c1;
    auto generator = Composite(c2, c3);
    auto result = generate(generator);
    static_assert(std::tuple_size_v<decltype(result)> == 2);
    REQUIRE(std::get<0>(result) == std::get<1>(result));
  }
}

TEST_CASE("test_complex", "[Composite]") {
  SECTION("Dependent ranges") {
    
    auto r1 = Reference(Range(Constant(5), Constant(10)));
    auto r2 = Reference(Range(Constant(30), Constant(35)));
    auto r3 = Reference(Range(Constant(25), r2));
    auto r4 = Reference(Range(r1, r3));
    auto generator = Composite(r2, r4);
    
    for(int i = 0; i < 100; ++i) {
      auto result = generate(generator);
      auto right_bound = std::get<0>(result);
      REQUIRE(right_bound >= 30);
      REQUIRE(right_bound <= 35);
      auto val = std::get<1>(result);
      REQUIRE(val >= 5);
      REQUIRE(val <= right_bound);
    }
  }
}

TEST_CASE("test_dangling_references", "[Composite]") {
  SECTION("Copy references") {
    auto generator = [] {
      auto r1 = Reference(Range(Constant(5), Constant(10)));
      auto r2 = Reference(Range(Constant(30), Constant(35)));
      auto r3 = Reference(Range(Constant(25), r2));
      auto r4 = Reference(Range(r1, r3));
      return Composite(r2, r4);
    }();
    for (int i = 0; i < 100; ++i) {
      auto result = generate(generator);
      auto right_bound = std::get<0>(result);
      REQUIRE(right_bound >= 30);
      REQUIRE(right_bound <= 35);
      auto val = std::get<1>(result);
      REQUIRE(val >= 5);
      REQUIRE(val <= right_bound);
    }
  }
  SECTION("Emplace references") {
    auto generator = [] {
      auto dep = Reference(Range(Constant(30), Constant(35)));
      return Composite(dep, Range(Range(Constant(5), Constant(10)),
        Range(Constant(25), dep)));
    }();
    for(int i = 0; i < 100; ++i) {
      auto result = generate(generator);
      auto right_bound = std::get<0>(result);
      REQUIRE(right_bound >= 30);
      REQUIRE(right_bound <= 35);
      auto val = std::get<1>(result);
      REQUIRE(val >= 5);
      REQUIRE(val <= right_bound);
    }
  }
}

TEST_CASE("test_incompatible_types", "[Composite]") {
  struct TestType {
    TestType(int x) : value(x) {}
    int value;
  };

  auto r1 = Reference(Range(Constant(5), Constant(10)));
  auto r2 = Reference(Constant(11.f));
  auto r3 = Reference(Constant<std::string>("abc"));
  auto r4 = Reference(Constant<TestType>(5));
  auto generator = Composite(r1, r2, r3, r4);

  for(int i = 0; i < 100; ++i) {
    auto result = generate(generator);
    REQUIRE(std::get<0>(result) >= 5);
    REQUIRE(std::get<0>(result) <= 10);
    REQUIRE(std::get<1>(result) == 11.f);
    REQUIRE(std::get<2>(result) == "abc");
    REQUIRE(std::get<3>(result).value == 5);
  }
}
