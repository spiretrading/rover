#include <string>
#include <catch.hpp>
#include "Rover/Constant.hpp"
#include "Rover/Generator.hpp"
#include "Rover/Lift.hpp"
#include "Rover/Range.hpp"

using namespace Rover;
using namespace std::string_literals;

TEST_CASE("test_independent", "[Lift]") {
  SECTION("Reduce") {
    auto c1 = Constant(5);
    auto c2 = Constant(10);
    auto c3 = Constant(20);
    auto generator = Lift([](int v1, int v2, int v3) {
      return v1 + v2 + v3;
    }, c1, c2, c3);
    REQUIRE(generate(generator) == 35);
  }
  SECTION("Tuple") {
    auto c1 = Constant(5);
    auto c2 = Constant(10);
    auto c3 = Constant(20);
    auto generator = Lift([](int v1, int v2, int v3) {
      return std::make_tuple(v1 + 10, v2 + 10, v3 + 10);
    }, c1, c2, c3);
    auto result = generate(generator);
    REQUIRE(std::get<0>(result) == 15);
    REQUIRE(std::get<1>(result) == 20);
    REQUIRE(std::get<2>(result) == 30);
  }
}

TEST_CASE("test_dependent", "[Lift]") {
  SECTION("Constants") {
    auto c1 = Constant(5);
    auto c2 = c1;
    auto c3 = Constant(20);
    auto generator = Lift([](int v1, int v2, int v3, int v4) {
      return std::make_tuple(v1, v2, v3, v4);
    }, c1, c2, c3, c3);
    auto result = generate(generator);
    REQUIRE(std::get<0>(result) == 5);
    REQUIRE(std::get<1>(result) == 5);
    REQUIRE(std::get<2>(result) == 20);
    REQUIRE(std::get<3>(result) == 20);
  }
  SECTION("FlatRanges") {
    for(int i = 0; i < 100; ++i) {
      auto r1 = Range(Constant(5), Constant(10));
      auto r2 = r1;
      auto r3 = Range(Constant(20), Constant(30));
      auto generator = Lift([](int v1, int v2, int v3, int v4) {
        return std::make_tuple(v1, v2, v3, v4);
      }, r1, r2, r3, r3);
      auto result = generate(generator);
      REQUIRE(std::get<0>(result) == std::get<1>(result));
      REQUIRE(std::get<1>(result) != std::get<2>(result));
      REQUIRE(std::get<2>(result) == std::get<3>(result));
    }
  }
  SECTION("RangeOfRanges") {
    for(int i = 0; i < 100; ++i) {
      auto r1 = Range(Constant(5), Constant(10));
      auto r2 = Range(Constant(20), Constant(30));
      auto r3 = Range(r1, r2);
      auto generator = Lift([](int v1, int v2, int v3) {
        return std::make_tuple(v1, v2, v3);
      }, r1, r2, r3);
      auto result = generate(generator);
      REQUIRE(std::get<2>(result) >= std::get<0>(result));
      REQUIRE(std::get<2>(result) <= std::get<1>(result));
    }
  }
}

TEST_CASE("test_latent", "[Lift]") {
  SECTION("Constants") {
    auto c1 = Constant(5);
    auto c2 = c1;
    auto generator = Lift([](int v) {
      return v;
    }, c2);
    auto result = generate(generator);
    REQUIRE(result == 5);
  }
  SECTION("FlatRanges") {
    for(int i = 0; i < 100; ++i) {
      auto r1 = Range(Constant(5), Constant(10));
      auto r2 = r1;
      auto generator = Lift([](int v) {
        return v;
      }, r2);
      auto result = generate(generator);
      REQUIRE(result >= 5);
      REQUIRE(result <= 10);
    }
  }
  SECTION("RangeOfRanges") {
    for(int i = 0; i < 100; ++i) {
      auto r1 = Range(Constant(5), Constant(10));
      auto r2 = Range(Constant(20), Constant(30));
      auto r3 = Range(r1, r2);
      auto generator = Lift([](int v1, int v2) {
        return std::make_tuple(v1, v2);
      }, r1, r3);
      auto result = generate(generator);
      REQUIRE(std::get<0>(result) >= 5);
      REQUIRE(std::get<0>(result) <= 10);
      REQUIRE(std::get<1>(result) >= std::get<0>(result));
      REQUIRE(std::get<1>(result) <= 30);
    }
  }
  SECTION("SplitRanges") {
    for(int i = 0; i < 100; ++i) {
      auto r1 = Range(Constant(5), Constant(10));
      auto r2 = Range(Constant(20), Constant(30));
      auto r3 = Range(Constant(40), Constant(50));
      auto r4 = Range(r1, r2);
      auto r5 = Range(r2, r3);
      auto generator = Lift([](int v1, int v2) {
        return std::make_tuple(v1, v2);
      }, r4, r5);
      auto result = generate(generator);
      REQUIRE(std::get<0>(result) >= 5);
      REQUIRE(std::get<0>(result) <= std::get<1>(result));
      REQUIRE(std::get<1>(result) <= 50);
    }
  }
}

TEST_CASE("test_dangling", "[Lift]") {
  SECTION("DestroyOriginalConstant") {
    auto generator = Lift([](int v) {
      return v + 10;
    }, Constant(5));
    REQUIRE(generate(generator) == 15);
  }
  SECTION("DestroyOriginalRange") {
    for(int i = 0; i < 100; ++i) {
      auto generator = Lift([](int v) {
        return v + 10;
      }, Range(Constant(5), Constant(10)));
      auto result = generate(generator);
      REQUIRE(result >= 15);
      REQUIRE(result <= 20);
    }
  }
  SECTION("DestroyLatentRanges") {
    for(int i = 0; i < 100; ++i) {
      auto [r1, r2] = [] {
        auto r1 = Range(Constant(5), Constant(10));
        auto r2 = Range(Constant(20), Constant(30));
        auto r3 = Range(Constant(40), Constant(50));
        return std::make_pair(r1, r2);
      }();
      auto generator = Lift([](int v1, int v2) {
        return std::make_tuple(v1, v2);
      }, r1, r2);
      auto result = generate(generator);
      REQUIRE(std::get<0>(result) >= 5);
      REQUIRE(std::get<0>(result) <= std::get<1>(result));
      REQUIRE(std::get<1>(result) <= 50);
    }
  }
  SECTION("ReturnGenerator") {
    for(int i = 0; i < 100; ++i) {
      auto generator = [] {
        auto r1 = Range(Constant(5), Constant(10));
        auto r2 = Range(Constant(20), Constant(30));
        auto r3 = Range(Constant(40), Constant(50));
        auto r4 = Range(r1, r2);
        auto r5 = Range(r2, r3);
        return Lift([](int v1, int v2) {
          return std::make_tuple(v1, v2);
        }, r4, r5);
      }();
      auto result = generate(generator);
      REQUIRE(std::get<0>(result) >= 5);
      REQUIRE(std::get<0>(result) <= std::get<1>(result));
      REQUIRE(std::get<1>(result) <= 50);
    }
  }
}
