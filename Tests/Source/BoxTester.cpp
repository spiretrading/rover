#include <memory>
#include <string>
#include <catch.hpp>
#include "Rover/Box.hpp"
#include "Rover/Constant.hpp"
#include "Rover/Generator.hpp"
#include "Rover/Lift.hpp"
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
    for(int i = 0; i < 100; ++i) {
      auto result = generate(b);
      REQUIRE(result >= 5);
      REQUIRE(result <= 10);
    }
  }
}

TEST_CASE("test_raw_pointers", "[Box]") {
  SECTION("Constant.") {
    auto c = new Constant(5);
    auto b = Box(c);
    static_assert(std::is_same_v<std::decay_t<decltype(b)>, Box<int>>);
    REQUIRE(generate(b) == 5);
  }
  SECTION("Range.") {
    auto r = new Range(Constant(5), Constant(10));
    auto b = Box(r);
    static_assert(std::is_same_v<std::decay_t<decltype(b)>, Box<int>>);
    for(int i = 0; i < 100; ++i) {
      auto result = generate(b);
      REQUIRE(result >= 5);
      REQUIRE(result <= 10);
    }
  }
}

TEST_CASE("test_unique_pointers", "[Box]") {
  SECTION("Constant.") {
    auto c = std::make_unique<Constant<int>>(5);
    auto b = Box(std::move(c));
    static_assert(std::is_same_v<std::decay_t<decltype(b)>, Box<int>>);
    REQUIRE(generate(b) == 5);
  }
  SECTION("Range.") {
    auto r = std::make_unique<Range<Constant<int>, Constant<int>, void>>(5,
        10);
    auto b = Box(std::move(r));
    static_assert(std::is_same_v<std::decay_t<decltype(b)>, Box<int>>);
    for(int i = 0; i < 100; ++i) {
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
    for(int i = 0; i < 100; ++i) {
      auto result = generate(b);
      REQUIRE(result >= 5);
      REQUIRE(result <= 10);
    }
  }
}

TEST_CASE("test_dependent_generators", "[Box]") {
  SECTION("FlatRanges") {
    for(int i = 0; i < 100; ++i) {
      auto r1 = Box(std::make_shared<Range<Constant<int>, Constant<int>,
          void>>(Constant(5), Constant(10)));
      auto r2 = r1;
      auto r3 = Box(std::make_shared<Range<Constant<int>, Constant<int>,
          void>>(Constant(20), Constant(30)));
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
      auto r1 = Box(std::make_shared<Range<Constant<int>, Constant<int>,
          void>>(Constant(5), Constant(10)));
      auto r2 = Box(std::make_shared<Range<Constant<int>, Constant<int>,
          void>>(Constant(20), Constant(30)));
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

TEST_CASE("test_latent_generators", "[Box]") {
  SECTION("FlatRanges") {
    for(int i = 0; i < 100; ++i) {
      auto r1 = Box(std::make_shared<Range<Constant<int>, Constant<int>, void>>(
        Constant(5), Constant(10)));
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
      auto r1 = Box(std::make_shared<Range<Constant<int>, Constant<int>,
          void>>(Constant(5), Constant(10)));
      auto r2 = Box(std::make_shared<Range<Constant<int>, Constant<int>,
          void>>(Constant(20), Constant(30)));
      auto r3 = Box(Range(r1, r2));
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
      auto r1 = Box(std::make_shared<Range<Constant<int>, Constant<int>,
          void>>(Constant(5), Constant(10)));
      auto r2 = Box(std::make_shared<Range<Constant<int>, Constant<int>,
          void>>(Constant(20), Constant(30)));
      auto r3 = Box(std::make_shared<Range<Constant<int>, Constant<int>,
          void>>(Constant(40), Constant(50)));
      auto r4 = Box(Range(r1, r2));
      auto r5 = Box(Range(r2, r3));
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

TEST_CASE("test_dangling_generators", "[Box]") {
  SECTION("DestroyOriginalConstant") {
    auto generator = Lift([](int v) {
      return v + 10;
    }, Box(Constant(5)));
    REQUIRE(generate(generator) == 15);
  }
  SECTION("DestroyOriginalRange") {
    for(int i = 0; i < 100; ++i) {
      auto generator = Lift([](int v) {
        return v + 10;
      }, Box(Range(Constant(5), Constant(10))));
      auto result = generate(generator);
      REQUIRE(result >= 15);
      REQUIRE(result <= 20);
    }
  }
  SECTION("DestroyLatentRanges") {
    for(int i = 0; i < 100; ++i) {
      auto[r1, r2] = [] {
        auto r1 = Box(Range(Constant(5), Constant(10)));
        auto r2 = Box(Range(Constant(20), Constant(30)));
        auto r3 = Box(Range(Constant(40), Constant(50)));
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
        auto r1 = Box(std::make_shared<Range<Constant<int>, Constant<int>,
            void>>(Constant(5), Constant(10)));
        auto r2 = Box(std::make_shared<Range<Constant<int>, Constant<int>,
            void>>(Constant(20), Constant(30)));
        auto r3 = Box(std::make_shared<Range<Constant<int>, Constant<int>,
            void>>(Constant(40), Constant(50)));
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
