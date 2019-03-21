#include <catch.hpp>
#include "Rover/LinearRegressionModel.hpp"
#include "Rover/ListTrial.hpp"
#include "Rover/Sample.hpp"

using namespace Rover;

TEST_CASE("test_simple_linear_regression", "[LinearRegressionModel]") {
  SECTION("TwoPoints.") {
    auto trial = ListTrial<Sample<double, double>>();
    trial.insert({ 4., { 1. } });
    trial.insert({ 2., { 0. } });
    auto model = LinearRegressionModel(trial);
    REQUIRE(model(-10.) == Approx(-18.));
    REQUIRE(model(0.5) == Approx(3.));
    REQUIRE(model(10.) == Approx(22.));
  }
  SECTION("ThreePointsNoError.") {
    auto trial = ListTrial<Sample<double, double>>();
    trial.insert({ 4., { 1. } });
    trial.insert({ 2., { 0. } });
    trial.insert({ 8., { 3. } });
    auto model = LinearRegressionModel(trial);
    REQUIRE(model(-10.) == Approx(-18.));
    REQUIRE(model(0.5) == Approx(3.));
    REQUIRE(model(10.) == Approx(22.));
  }
  SECTION("ThreePointsWithError.") {
    auto trial = ListTrial<Sample<double, double>>();
    trial.insert({ 5., { 0. } });
    trial.insert({ 2., { 1. } });
    trial.insert({ 5., { 2. } });
    auto model = LinearRegressionModel(trial);
    REQUIRE(model(-10.) == Approx(4.));
    REQUIRE(model(-1.) == Approx(4.));
    REQUIRE(model(2.) == Approx(4.));
    REQUIRE(model(4.) == Approx(4.));
    REQUIRE(model(10.) == Approx(4.));
  }
}

TEST_CASE("test_multivariable_linear_regression", "[LinearRegressionModel]") {
  SECTION("ThreePoints.") {
    auto trial = ListTrial<Sample<double, double, double>>();
    trial.insert({ 2., { 1., 1. } });
    trial.insert({ 8., { 2., 4. } });
    trial.insert({ 16., { 4., 4. } });
    auto model = LinearRegressionModel(trial);
    REQUIRE(model(0., 0.) == Approx(-8. / 3));
    REQUIRE(model(1., 0.) == Approx(4. / 3));
    REQUIRE(model(0., 1.) == Approx(-2.));
    REQUIRE(model(1., 1.) == Approx(2.));
  }
  SECTION("FourPointsNoError.") {
    auto trial = ListTrial<Sample<double, double, double>>();
    trial.insert({ 2., { 1., 1. } });
    trial.insert({ 8., { 2., 4. } });
    trial.insert({ 16., { 4., 4. } });
    trial.insert({ 20. / 3, { 2., 2. } });
    auto model = LinearRegressionModel(trial);
    REQUIRE(model(0., 0.) == Approx(-8. / 3));
    REQUIRE(model(1., 0.) == Approx(4. / 3));
    REQUIRE(model(0., 1.) == Approx(-2.));
    REQUIRE(model(1., 1.) == Approx(2.));
  }
  SECTION("ForPointsWithError.") {
    auto trial = ListTrial<Sample<double, double, double>>();
    trial.insert({ 2., { 1., 3. } });
    trial.insert({ 4., { 4., 2.5 } });
    trial.insert({ -1., { 6., 0. } });
    trial.insert({ -2., { -1., 2. } });
    auto model = LinearRegressionModel(trial);
    REQUIRE(model(1., 0.) == Approx(-5.5104));
    REQUIRE(model(0., 0.) == Approx(-6.4302));
    REQUIRE(model(0., 1.) == Approx(-3.8271));
    REQUIRE(model(1., 1.) == Approx(-2.9073));
    REQUIRE(model(10., 10.) == Approx(28.7981));
    REQUIRE(model(-10., -10.) == Approx(-41.6584));
  }
}
