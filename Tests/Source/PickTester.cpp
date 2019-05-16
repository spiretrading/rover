#include <catch.hpp>
#include "Rover/Constant.hpp"
#include "Rover/Evaluator.hpp"
#include "Rover/Generator.hpp"
#include "Rover/Pick.hpp"

using namespace Rover;

TEST_CASE("test_pick_fixed_index", "[Pick]") {
  SECTION("Single generator.") {
    struct Choice {
      public:
        using Type = std::size_t;

        Type generate(Evaluator& evaluator) {
          return 0;
        }
    };
    auto pick = Pick(Choice(), Constant(5));
    REQUIRE(generate(pick) == 5);
  }
  SECTION("Multiple generators.") {
    struct Choice {
      public:
        using Type = std::size_t;

        Type generate(Evaluator& evaluator) {
          return 1;
        }
    };
    auto c1 = Constant(3);
    auto c2 = Constant(5);
    auto c3 = Constant(7);
    auto pick = Pick(Choice(), c1, c2, c3);
    REQUIRE(generate(pick) == 5);
  }
}

TEST_CASE("test_pick_variable_index", "[Pick]") {
  struct Choice {
    public:
      using Type = std::size_t;

      Choice()
        : m_counter(0) {}

      Type generate(Evaluator& evaluator) {
        auto index = m_counter % 3;
        ++m_counter;
        return index;
      }

    private:
      std::size_t m_counter;
  };
  auto c1 = Constant(3);
  auto c2 = Constant(5);
  auto c3 = Constant(7);
  auto i = std::size_t(0);
  auto pick = Pick(Choice(), c1, c2, c3);
  REQUIRE(generate(pick) == 3);
  REQUIRE(generate(pick) == 5);
  REQUIRE(generate(pick) == 7);
  REQUIRE(generate(pick) == 3);
  REQUIRE(generate(pick) == 5);
  REQUIRE(generate(pick) == 7);
  REQUIRE(generate(pick) == 3);
}
