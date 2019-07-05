#include <numeric>
#include <catch2/catch.hpp>
#include "Rover/Factor.hpp"

using namespace Rover;

namespace {
  struct DefaultWrapper {
    bool operator ==(DefaultWrapper other) const {
      return m_value == other.m_value;
    }

    int m_value;
  };

  struct ComparableWrapper {
    bool operator <(ComparableWrapper other) const {
      return m_value < other.m_value;
    }

    int m_value;
  };

  template<typename Factor, typename... Categories>
  bool check_categories(const Factor& factor, Categories&&... categories) {
    auto left_categories = sizeof...(categories);
    auto found = std::vector<bool>(left_categories, false);
    ([&](auto& category) {
      auto dimension = factor.find_dimension(category);
      REQUIRE(dimension);
      if(!found[*dimension]) {
        --left_categories;
        found[*dimension] = true;
      }
    }(categories), ...);
    return left_categories == 0;
  }
}

TEST_CASE("test_default_factor", "[Factor]") {
  SECTION("No repetitions.") {
    auto factor = Factor<DefaultWrapper>();
    factor.add_category({ 5 });
    factor.add_category({ 3 });
    factor.add_category({ 7 });
    REQUIRE(check_categories(factor, DefaultWrapper{ 5 }, DefaultWrapper{ 3 },
      DefaultWrapper{ 7 }));
  }
  SECTION("Repeated categories.") {
    auto factor = Factor<DefaultWrapper>();
    factor.add_category({ 5 });
    factor.add_category({ 3 });
    factor.add_category({ 5 });
    factor.add_category({ 3 });
    factor.add_category({ 3 });
    factor.add_category({ 7 });
    REQUIRE(check_categories(factor, DefaultWrapper{ 5 }, DefaultWrapper{ 3 },
      DefaultWrapper{ 7 }));
  }
}

TEST_CASE("test_tree_factor", "[Factor]") {
  SECTION("No repetitions.") {
    auto factor = Factor<ComparableWrapper>();
    factor.add_category({ 5 });
    factor.add_category({ 3 });
    factor.add_category({ 7 });
    REQUIRE(check_categories(factor, ComparableWrapper{ 5 },
      ComparableWrapper{ 3 }, ComparableWrapper{ 7 }));
  }
  SECTION("Repeated categories.") {
    auto factor = Factor<ComparableWrapper>();
    factor.add_category({ 5 });
    factor.add_category({ 3 });
    factor.add_category({ 5 });
    factor.add_category({ 3 });
    factor.add_category({ 3 });
    factor.add_category({ 7 });
    REQUIRE(check_categories(factor, ComparableWrapper{ 5 },
      ComparableWrapper{ 3 }, ComparableWrapper{ 7 }));
  }
}

TEST_CASE("test_hash_factor", "[Factor]") {
  SECTION("No repetitions.") {
    auto factor = Factor<int>();
    factor.add_category(5);
    factor.add_category(3);
    factor.add_category(7);
    REQUIRE(check_categories(factor, 5, 3, 7));
  }
  SECTION("Repeated categories.") {
    auto factor = Factor<int>();
    factor.add_category(5);
    factor.add_category(3);
    factor.add_category(5);
    factor.add_category(3);
    factor.add_category(3);
    factor.add_category(7);
    REQUIRE(check_categories(factor, 5, 3, 7));
  }
}
