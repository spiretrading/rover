#include <type_traits>
#include <catch.hpp>
#include "Rover/LinearRegressionModel.hpp"
#include "Rover/ListTrial.hpp"
#include "Rover/Sample.hpp"

using namespace Rover;

namespace {
  template<typename T>
  struct Conv {
    Conv(T val)
      : m_val(std::move(val)) {}

    operator T() const {
      return m_val;
    }

    T m_val;
  };
}

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
  SECTION("ConvertibleSameType.") {
    auto trial = ListTrial<Sample<Conv<double>, Conv<double>>>();
    trial.insert({ Conv<double>(4.), { Conv<double>(1.) } });
    trial.insert({ Conv<double>(2.), { Conv<double>(0.) } });
    trial.insert({ Conv<double>(8.), { Conv<double>(3.) } });
    auto model = LinearRegressionModel(trial);
    REQUIRE(static_cast<double>(model(Conv<double>(-10.))) == Approx(-18.));
    REQUIRE(static_cast<double>(model(Conv<double>(0.5))) == Approx(3.));
    REQUIRE(static_cast<double>(model(Conv<double>(10.))) == Approx(22.));
  }
  SECTION("ConvertibleMixedTypes.") {
    auto trial = ListTrial<Sample<Conv<float>, Conv<double>>>();
    trial.insert({ Conv<float>(4.f), { Conv<double>(1.) } });
    trial.insert({ Conv<float>(2.f), { Conv<double>(0.) } });
    trial.insert({ Conv<float>(8.f), { Conv<double>(3.) } });
    auto model = LinearRegressionModel(trial);
    REQUIRE(static_cast<float>(model(Conv<double>(-10.))) == Approx(-18.f));
    REQUIRE(static_cast<float>(model(Conv<double>(0.5))) == Approx(3.f));
    REQUIRE(static_cast<float>(model(Conv<double>(10.))) == Approx(22.f));
  }
  SECTION("NarrowCompType.") {
    auto trial = ListTrial<Sample<double, double>>();
    trial.insert({ 4., { 1. } });
    trial.insert({ 2., { 0. } });
    auto model = LinearRegressionModel<std::decay_t<decltype(trial)>, float>(
      trial);
    REQUIRE(model(-10.) == Approx(-18.));
    REQUIRE(model(0.5) == Approx(3.));
    REQUIRE(model(10.) == Approx(22.));
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
  SECTION("ConvertibleSameType.") {
    auto trial = ListTrial<Sample<Conv<double>, Conv<double>, Conv<double>>>();
    trial.insert({ Conv<double>(2.), { Conv<double>(1.), Conv<double>(1.) } });
    trial.insert({ Conv<double>(8.), { Conv<double>(2.), Conv<double>(4.) } });
    trial.insert({ Conv<double>(16.), { Conv<double>(4.),
      Conv<double>(4.) } });
    auto model = LinearRegressionModel(trial);
    REQUIRE(static_cast<double>(model(Conv<double>(0.), Conv<double>(0.))) ==
      Approx(-8. / 3));
    REQUIRE(static_cast<double>(model(Conv<double>(1.), Conv<double>(0.))) ==
      Approx(4. / 3));
    REQUIRE(static_cast<double>(model(Conv<double>(0.), Conv<double>(1.))) ==
      Approx(-2.));
    REQUIRE(static_cast<double>(model(Conv<double>(1.), Conv<double>(1.))) ==
      Approx(2.));
  }
  SECTION("ConvertibleMixedTypes.") {
    auto trial = ListTrial<Sample<Conv<double>, Conv<float>, Conv<double>>>();
    trial.insert({ Conv<double>(2.), { Conv<float>(1.f), Conv<double>(1.) } });
    trial.insert({ Conv<double>(8.), { Conv<float>(2.f), Conv<double>(4.) } });
    trial.insert({ Conv<double>(16.), { Conv<float>(4.f),
      Conv<double>(4.) } });
    auto model = LinearRegressionModel(trial);
    REQUIRE(static_cast<double>(model(Conv<float>(0.f), Conv<double>(0.))) ==
      Approx(-8. / 3));
    REQUIRE(static_cast<double>(model(Conv<float>(1.f), Conv<double>(0.))) ==
      Approx(4. / 3));
    REQUIRE(static_cast<double>(model(Conv<float>(0.f), Conv<double>(1.))) ==
      Approx(-2.));
    REQUIRE(static_cast<double>(model(Conv<float>(1.f), Conv<double>(1.))) ==
      Approx(2.));
  }
  SECTION("NarrowCompType.") {
    auto trial = ListTrial<Sample<double, double, double>>();
    trial.insert({ 2., { 1., 1. } });
    trial.insert({ 8., { 2., 4. } });
    trial.insert({ 16., { 4., 4. } });
    auto model = LinearRegressionModel<std::decay_t<decltype(trial)>, float>(
      trial);
    REQUIRE(model(0., 0.) == Approx(-8. / 3));
    REQUIRE(model(1., 0.) == Approx(4. / 3));
    REQUIRE(model(0., 1.) == Approx(-2.));
    REQUIRE(model(1., 1.) == Approx(2.));
  }
}
