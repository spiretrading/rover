#include <map>
#include <set>
#include <vector>
#include <catch.hpp>
#include "Rover/Constant.hpp"
#include "Rover/Evaluator.hpp"
#include "Rover/Generator.hpp"
#include "Rover/Select.hpp"

using namespace Rover;

TEST_CASE("test_array_select", "[Select]") {
  SECTION("Default selector.") {
    auto select = Select(std::vector<Constant<int>>{ 1, 3, 5 });
    for(int i = 0; i < 100; ++i) {
      auto value = generate(select);
      REQUIRE((value == 1 || value == 3 || value == 5));
    }
  }
  SECTION("User-defined selector.") {
    {
      auto select = Select(std::vector<Constant<int>>{ 1, 3, 5 }, Constant(0));
      auto value = generate(select);
      REQUIRE(value == 1);
    }
    {
      auto select = Select(std::vector<Constant<int>>{ 1, 3, 5 }, Constant(1));
      auto value = generate(select);
      REQUIRE(value == 3);
    }
    {
      auto select = Select(std::vector<Constant<int>>{ 1, 3, 5 }, Constant(2));
      auto value = generate(select);
      REQUIRE(value == 5);
    }
  }
}

TEST_CASE("test_set_select", "[Select]") {
  SECTION("Default selector.") {
    auto select = Select(std::set<int>{ 1, 3, 5 });
    for(int i = 0; i < 100; ++i) {
      auto value = generate(select);
      REQUIRE((value == 1 || value == 3 || value == 5));
    }
  }
  SECTION("User-defined selector.") {
    {
      auto select = Select(std::set<int>{ 3, 5, 1 }, Constant(0));
      auto value = generate(select);
      REQUIRE(value == 1);
    }
    {
      auto select = Select(std::set<int>{ 3, 5, 1 }, Constant(1));
      auto value = generate(select);
      REQUIRE(value == 3);
    }
    {
      auto select = Select(std::set<int>{ 3, 5, 1 }, Constant(2));
      auto value = generate(select);
      REQUIRE(value == 5);
    }
  }
}

TEST_CASE("test_map_select", "[Select]") {
  SECTION("Default selector.") {
    auto select = Select(std::map<int, int>{ { 3, 2 }, { 5, 4 }, { 1, 6 } });
    for(int i = 0; i < 100; ++i) {
      auto value = generate(select);
      REQUIRE((value == 2 || value == 4 || value == 6));
    }
  }
  SECTION("User-defined selector.") {
    {
      auto select = Select(std::map<int, int>{ { 3, 2 }, { 5, 4 }, { 1, 6 } }, Constant(0));
      auto value = generate(select);
      REQUIRE(value == 6);
    }
    {
      auto select = Select(std::map<int, int>{ { 3, 2 }, { 5, 4 }, { 1, 6 } }, Constant(1));
      auto value = generate(select);
      REQUIRE(value == 2);
    }
    {
      auto select = Select(std::map<int, int>{ { 3, 2 }, { 5, 4 }, { 1, 6 } }, Constant(2));
      auto value = generate(select);
      REQUIRE(value == 4);
    }
  }
}
