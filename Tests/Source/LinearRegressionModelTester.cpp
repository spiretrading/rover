#include <catch2/catch.hpp>
#include "Rover/LinearRegression.hpp"
#include "Rover/ListTrial.hpp"
#include "Rover/Model.hpp"
#include "Rover/Sample.hpp"

using namespace Rover;

namespace {
  template<typename T>
  struct Convertible {
    Convertible(T val)
      : m_val(std::move(val)) {}

    operator T() const {
      return m_val;
    }

    T m_val;
  };
}

TEST_CASE("test_simple_linear_regression_model", "[LinearRegressionModel]") {
  SECTION("TwoPoints.") {
    auto trial = ListTrial<Sample<double, double>>();
    trial.insert({ 4., { 1. } });
    trial.insert({ 2., { 0. } });
    auto model = Model<LinearRegression<double>, ListTrial<Sample<double,
      double>>>(trial);
    REQUIRE(model(-10.) == Approx(-18.));
    REQUIRE(model(0.5) == Approx(3.));
    REQUIRE(model(10.) == Approx(22.));
  }
  SECTION("ThreePointsNoError.") {
    auto trial = ListTrial<Sample<double, double>>();
    trial.insert({ 4., { 1. } });
    trial.insert({ 2., { 0. } });
    trial.insert({ 8., { 3. } });
    auto model = Model<LinearRegression<double>, ListTrial<Sample<double,
      double>>>(trial);
    REQUIRE(model(-10.) == Approx(-18.));
    REQUIRE(model(0.5) == Approx(3.));
    REQUIRE(model(10.) == Approx(22.));
  }
  SECTION("ThreePointsWithError.") {
    auto trial = ListTrial<Sample<double, double>>();
    trial.insert({ 5., { 0. } });
    trial.insert({ 2., { 1. } });
    trial.insert({ 5., { 2. } });
    auto model = Model<LinearRegression<double>, ListTrial<Sample<double,
      double>>>(trial);
    REQUIRE(model(-10.) == Approx(4.));
    REQUIRE(model(-1.) == Approx(4.));
    REQUIRE(model(2.) == Approx(4.));
    REQUIRE(model(4.) == Approx(4.));
    REQUIRE(model(10.) == Approx(4.));
  }
  SECTION("ConvertibleSameType.") {
    auto trial = ListTrial<Sample<Convertible<double>, Convertible<double>>>();
    trial.insert({ Convertible<double>(4.), { Convertible<double>(1.) } });
    trial.insert({ Convertible<double>(2.), { Convertible<double>(0.) } });
    trial.insert({ Convertible<double>(8.), { Convertible<double>(3.) } });
    auto model = Model<LinearRegression<double>, ListTrial<Sample<Convertible<
      double>, Convertible<double>>>>(trial);
    REQUIRE(static_cast<double>(model(Convertible<double>(-10.))) ==
      Approx(-18.));
    REQUIRE(static_cast<double>(model(Convertible<double>(0.5))) ==
      Approx(3.));
    REQUIRE(static_cast<double>(model(Convertible<double>(10.))) ==
      Approx(22.));
  }
  SECTION("ConvertibleMixedTypes.") {
    auto trial = ListTrial<Sample<Convertible<float>, Convertible<double>>>();
    trial.insert({ Convertible<float>(4.f), { Convertible<double>(1.) } });
    trial.insert({ Convertible<float>(2.f), { Convertible<double>(0.) } });
    trial.insert({ Convertible<float>(8.f), { Convertible<double>(3.) } });
    auto model = Model<LinearRegression<double>, ListTrial<Sample<Convertible<
      float>, Convertible<double>>>>(trial);
    REQUIRE(static_cast<float>(model(Convertible<double>(-10.))) ==
      Approx(-18.f));
    REQUIRE(static_cast<float>(model(Convertible<double>(0.5))) ==
      Approx(3.f));
    REQUIRE(static_cast<float>(model(Convertible<double>(10.))) ==
      Approx(22.f));
  }
  SECTION("NarrowComputationType.") {
    auto trial = ListTrial<Sample<double, double>>();
    trial.insert({ 4., { 1. } });
    trial.insert({ 2., { 0. } });
    auto model = Model<LinearRegression<float>, ListTrial<Sample<double,
      double>>>(trial);
    REQUIRE(model(-10.) == Approx(-18.));
    REQUIRE(model(0.5) == Approx(3.));
    REQUIRE(model(10.) == Approx(22.));
  }
}

TEST_CASE("test_multivariable_linear_regression_model",
    "[LinearRegressionModel]") {
  SECTION("ThreePoints.") {
    auto trial = ListTrial<Sample<double, double, double>>();
    trial.insert({ 2., { 1., 1. } });
    trial.insert({ 8., { 2., 4. } });
    trial.insert({ 16., { 4., 4. } });
    auto model = Model<LinearRegression<double>, ListTrial<Sample<double,
      double, double>>>(trial);
    REQUIRE(model({ 0., 0. }) == Approx(-8. / 3));
    REQUIRE(model({ 1., 0. }) == Approx(4. / 3));
    REQUIRE(model({ 0., 1. }) == Approx(-2.));
    REQUIRE(model({ 1., 1. }) == Approx(2.));
  }
  SECTION("FourPointsNoError.") {
    auto trial = ListTrial<Sample<double, double, double>>();
    trial.insert({ 2., { 1., 1. } });
    trial.insert({ 8., { 2., 4. } });
    trial.insert({ 16., { 4., 4. } });
    trial.insert({ 20. / 3, { 2., 2. } });
    auto model = Model<LinearRegression<double>, ListTrial<Sample<double,
      double, double>>>(trial);
    REQUIRE(model({ 0., 0. }) == Approx(-8. / 3));
    REQUIRE(model({ 1., 0. }) == Approx(4. / 3));
    REQUIRE(model({ 0., 1. }) == Approx(-2.));
    REQUIRE(model({ 1., 1. }) == Approx(2.));
  }
  SECTION("ForPointsWithError.") {
    auto trial = ListTrial<Sample<double, double, double>>();
    trial.insert({ 2., { 1., 3. } });
    trial.insert({ 4., { 4., 2.5 } });
    trial.insert({ -1., { 6., 0. } });
    trial.insert({ -2., { -1., 2. } });
    auto model = Model<LinearRegression<double>, ListTrial<Sample<double,
      double, double>>>(trial);
    REQUIRE(model({ 1., 0. }) == Approx(-5.5104));
    REQUIRE(model({ 0., 0. }) == Approx(-6.4302));
    REQUIRE(model({ 0., 1. }) == Approx(-3.8271));
    REQUIRE(model({ 1., 1. }) == Approx(-2.9073));
    REQUIRE(model({ 10., 10. }) == Approx(28.7981));
    REQUIRE(model({ -10., -10. }) == Approx(-41.6584));
  }
  SECTION("ConvertibleSameType.") {
    auto trial = ListTrial<Sample<Convertible<double>, Convertible<double>,
      Convertible<double>>>();
    trial.insert({ Convertible<double>(2.), { Convertible<double>(1.),
      Convertible<double>(1.) } });
    trial.insert({ Convertible<double>(8.), { Convertible<double>(2.),
      Convertible<double>(4.) } });
    trial.insert({ Convertible<double>(16.), { Convertible<double>(4.),
      Convertible<double>(4.) } });
    auto model = Model<LinearRegression<double>, ListTrial<Sample<Convertible<
      double>, Convertible<double>, Convertible<double>>>>(trial);
    REQUIRE(static_cast<double>(model({ Convertible<double>(0.),
      Convertible<double>(0.) })) == Approx(-8. / 3));
    REQUIRE(static_cast<double>(model({ Convertible<double>(1.),
      Convertible<double>(0.) })) == Approx(4. / 3));
    REQUIRE(static_cast<double>(model({ Convertible<double>(0.),
      Convertible<double>(1.) })) == Approx(-2.));
    REQUIRE(static_cast<double>(model({ Convertible<double>(1.),
      Convertible<double>(1.) })) == Approx(2.));
  }
  SECTION("ConvertibleMixedTypes.") {
    auto trial = ListTrial<Sample<Convertible<double>, Convertible<float>,
      Convertible<double>>>();
    trial.insert({ Convertible<double>(2.), { Convertible<float>(1.f),
      Convertible<double>(1.) } });
    trial.insert({ Convertible<double>(8.), { Convertible<float>(2.f),
      Convertible<double>(4.) } });
    trial.insert({ Convertible<double>(16.), { Convertible<float>(4.f),
      Convertible<double>(4.) } });
    auto model = Model<LinearRegression<double>, ListTrial<Sample<Convertible<
      double>, Convertible<float>, Convertible<double>>>>(trial);
    REQUIRE(static_cast<double>(model({ Convertible<float>(0.f),
      Convertible<double>(0.) })) == Approx(-8. / 3));
    REQUIRE(static_cast<double>(model({ Convertible<float>(1.f),
      Convertible<double>(0.) })) == Approx(4. / 3));
    REQUIRE(static_cast<double>(model({ Convertible<float>(0.f),
      Convertible<double>(1.) })) == Approx(-2.));
    REQUIRE(static_cast<double>(model({ Convertible<float>(1.f),
      Convertible<double>(1.) })) == Approx(2.));
  }
  SECTION("NarrowComputationType.") {
    auto trial = ListTrial<Sample<double, double, double>>();
    trial.insert({ 2., { 1., 1. } });
    trial.insert({ 8., { 2., 4. } });
    trial.insert({ 16., { 4., 4. } });
    auto model = Model<LinearRegression<float>, ListTrial<Sample<double,
      double, double>>>(trial);
    REQUIRE(model({ 0., 0. }) == Approx(-8. / 3));
    REQUIRE(model({ 1., 0. }) == Approx(4. / 3));
    REQUIRE(model({ 0., 1. }) == Approx(-2.));
    REQUIRE(model({ 1., 1. }) == Approx(2.));
  }
}

TEST_CASE("test_categorical_linear_regression_model",
    "[LinearRegressionModel]") {
  SECTION("Categorical only.") {
    using Sample = Rover::Sample<double, std::string>;
    auto trial = ListTrial<Sample>();
    trial.insert({ 7., { "A" } });
    trial.insert({ 5., { "B" } });
    trial.insert({ 3., { "C" } });
    trial.insert({ 2., { "A" } });
    trial.insert({ 9., { "B" } });
    trial.insert({ 0., { "C" } });
    auto model = Model<LinearRegression<double>, ListTrial<Sample>>(trial);
    REQUIRE(model({ "A" }) == Approx(4.5));
    REQUIRE(model({ "B" }) == Approx(7.));
    REQUIRE(model({ "C" }) == Approx(1.5));
  }
  SECTION("Mixed.") {
    using Sample = Rover::Sample<double, std::string, double>;
    auto trial = ListTrial<Sample>();
    trial.insert({ 7., { "A", 1. } });
    trial.insert({ 5., { "B", 1. } });
    trial.insert({ 3., { "C", 1. } });
    trial.insert({ 8., { "A", 2. } });
    trial.insert({ 6., { "B", 2. } });
    trial.insert({ 4., { "C", 2. } });
    auto model = Model<LinearRegression<double>, ListTrial<Sample>>(trial);
    REQUIRE(model({ "A", 3. }) == Approx(9.));
    REQUIRE(model({ "B", 3. }) == Approx(7.));
    REQUIRE(model({ "C", 3. }) == Approx(5.));
  }
}
