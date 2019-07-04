#include <catch2/catch.hpp>
#include "Rover/LinearRegression.hpp"
#include "Rover/ScalarView.hpp"

using namespace Rover;
using Arguments = ScalarSample<double>::Arguments;

TEST_CASE("test_simple_linear_regression", "[LinearRegression]") {
  SECTION("TwoPoints.") {
    auto view = ScalarView([](std::size_t i) {
      if(i == 0) {
        return ScalarSample<double>{ 4., { 1. } };
      } else {
        return ScalarSample<double>{ 2., { 0. } };
      }
    }, 2);
    auto a = LinearRegression<>();
    a.learn(view);
    REQUIRE(a.predict(Arguments{ -10. }) == Approx(-18.));
    REQUIRE(a.predict(Arguments{ 0.5 }) == Approx(3.));
    REQUIRE(a.predict(Arguments{ 10. }) == Approx(22.));
  }
  SECTION("ThreePointsNoError.") {
    auto view = ScalarView([](std::size_t i) {
      if(i == 0) {
        return ScalarSample<double>{ 4., { 1. } };
      } else if(i == 1) {
        return ScalarSample<double>{ 2., { 0. } };
      } else {
        return ScalarSample<double>{ 8., { 3. } };
      }
    }, 3);
    auto a = LinearRegression<>();
    a.learn(view);
    REQUIRE(a.predict(Arguments{ -10. }) == Approx(-18.));
    REQUIRE(a.predict(Arguments{ 0.5 }) == Approx(3.));
    REQUIRE(a.predict(Arguments{ 10. }) == Approx(22.));
  }
  SECTION("ThreePointsWithError.") {
    auto view = ScalarView([](std::size_t i) {
      if(i == 0) {
        return ScalarSample<double>{ 5., { 0. } };
      } else if(i == 1) {
        return ScalarSample<double>{ 2., { 1. } };
      } else {
        return ScalarSample<double>{ 5., { 2. } };
      }
    }, 3);
    auto a = LinearRegression<>();
    a.learn(view);
    REQUIRE(a.predict(Arguments{ -10. }) == Approx(4.));
    REQUIRE(a.predict(Arguments{ -1. }) == Approx(4.));
    REQUIRE(a.predict(Arguments{ 2. }) == Approx(4.));
    REQUIRE(a.predict(Arguments{ 4. }) == Approx(4.));
    REQUIRE(a.predict(Arguments{ 10. }) == Approx(4.));
  }
}

TEST_CASE("test_multivariable_linear_regression", "[LinearRegression]") {
  SECTION("ThreePoints.") {
    auto view = ScalarView([](std::size_t i) {
      if(i == 0) {
        return ScalarSample<double>{ 2., { 1., 1. } };
      } else if(i == 1) {
        return ScalarSample<double>{ 8., { 2., 4. } };
      } else {
        return ScalarSample<double>{ 16., { 4., 4. } };
      }
    }, 3);
    auto a = LinearRegression<>();
    a.learn(view);
    REQUIRE(a.predict(Arguments{ 0., 0. }) == Approx(-8. / 3));
    REQUIRE(a.predict(Arguments{ 1., 0. }) == Approx(4. / 3));
    REQUIRE(a.predict(Arguments{ 0., 1. }) == Approx(-2.));
    REQUIRE(a.predict(Arguments{ 1., 1. }) == Approx(2.));
  }
  SECTION("FourPointsNoError.") {
    auto view = ScalarView([](std::size_t i) {
      if(i == 0) {
        return ScalarSample<double>{ 2., { 1., 1. } };
      } else if(i == 1) {
        return ScalarSample<double>{ 8., { 2., 4. } };
      } else if(i == 2) {
        return ScalarSample<double>{ 16., { 4., 4. } };
      } else {
        return ScalarSample<double>{ 20. / 3, { 2., 2. } };
      }
    }, 4);
    auto a = LinearRegression<>();
    a.learn(view);
    REQUIRE(a.predict(Arguments{ 0., 0. }) == Approx(-8. / 3));
    REQUIRE(a.predict(Arguments{ 1., 0. }) == Approx(4. / 3));
    REQUIRE(a.predict(Arguments{ 0., 1. }) == Approx(-2.));
    REQUIRE(a.predict(Arguments{ 1., 1. }) == Approx(2.));
  }
  SECTION("ForPointsWithError.") {
    auto view = ScalarView([](std::size_t i) {
      if(i == 0) {
        return ScalarSample<double>{ 2., { 1., 3. } };
      } else if(i == 1) {
        return ScalarSample<double>{ 4., { 4., 2.5 } };
      } else if(i == 2) {
        return ScalarSample<double>{ -1., { 6., 0. } };
      } else {
        return ScalarSample<double>{ -2., { -1., 2. } };
      }
    }, 4);
    auto a = LinearRegression<>();
    a.learn(view);
    REQUIRE(a.predict(Arguments{ 1., 0. }) == Approx(-5.5104));
    REQUIRE(a.predict(Arguments{ 0., 0. }) == Approx(-6.4302));
    REQUIRE(a.predict(Arguments{ 0., 1. }) == Approx(-3.8271));
    REQUIRE(a.predict(Arguments{ 1., 1. }) == Approx(-2.9073));
    REQUIRE(a.predict(Arguments{ 10., 10. }) == Approx(28.7981));
    REQUIRE(a.predict(Arguments{ -10., -10. }) == Approx(-41.6584));
  }
}
