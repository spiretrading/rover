#include <algorithm>
#include <chrono>
#include <optional>
#include <catch2/catch.hpp>
#include "Rover/ListTrial.hpp"
#include "Rover/Model.hpp"
#include "Rover/Sample.hpp"
#include "Rover/ScalarView.hpp"

using namespace Rover;
using namespace std::chrono_literals;

namespace {
  class TestAlgorithm {
    public:
      using Type = double;
      using Sample = ScalarSample<double>;
      using Arguments = typename Sample::Arguments;
    
      template<typename ScalarView>
      void learn(const ScalarView& reader) {
        for(auto& sample : reader) {
          m_data.push_back(sample);
        }
        if(m_data.empty()) {
          return;
        }
        compute_norm();
        normalize_data();
      }
    
      double predict(Arguments arguments) const {
        normalize_arguments(arguments);
        struct Candidate {
          double distance;
          double result;
        };
        auto candidate1 = std::optional<Candidate>();
        auto candidate2 = std::optional<Candidate>();
        for(auto& sample : m_data) {
          auto distance = calculate_squared_distance(sample.m_arguments,
            arguments);
          if(!candidate1) {
            candidate1 = { distance, sample.m_result };
          } else if(distance < candidate1->distance) {
            candidate2 = candidate1;
            candidate1 = { distance, sample.m_result };
          } else if(!candidate2 || distance < candidate2->distance) {
            candidate2 = { distance, sample.m_result };
          }
        }
        if(!candidate2) {
          return candidate1->result;
        } else {
          return (candidate1->result + candidate2->result) / 2.;
        }
      }
    
    private:
      std::vector<Sample> m_data;
      Arguments m_min_values;
      Arguments m_max_values;

      void compute_norm() {
        m_min_values = m_data[0].m_arguments;
        m_max_values = m_data[0].m_arguments;
        for(auto i = std::size_t(0); i < m_data[0].m_arguments.size(); ++i) {
          auto [min_it, max_it] = std::minmax_element(m_data.begin(),
              m_data.end(), [&](const auto& lhs, const auto& rhs) {
            return lhs.m_arguments[i] < rhs.m_arguments[i];
          });
          m_min_values[i] = min_it->m_arguments[i];
          m_max_values[i] = max_it->m_arguments[i];
        }
      }
    
      void normalize_data() {
        for(auto& sample : m_data) {
          normalize_arguments(sample.m_arguments);
        }
      }
    
      void normalize_arguments(Arguments& arguments) const {
        for(auto i = std::size_t(0); i < m_min_values.size(); ++i) {
          if(m_min_values[i] == m_max_values[i]) {
            arguments[i] = 0.;
          } else {
            arguments[i] -= m_min_values[i];
            arguments[i] /= m_max_values[i] - m_min_values[i];
          }
        }
      }
    
      static double calculate_squared_distance(const Arguments& lhs,
          const Arguments& rhs) {
        auto result = 0.;
        for(auto i = std::size_t(0); i < lhs.size(); ++i) {
          result += (lhs[i] - rhs[i]) * (lhs[i] - rhs[i]);
        }
        return result;
      }
  };

  template<int>
  class DoubleWrapper {
    public:
      DoubleWrapper(double value)
        : m_value(value) {}

      explicit operator double() const {
        return m_value;
      }

      DoubleWrapper operator *(double multiplier) const {
        auto copy = *this;
        copy *= multiplier;
        return copy;
      }

      double operator /(DoubleWrapper other) const {
        return m_value / other.m_value;
      }

      DoubleWrapper& operator +=(DoubleWrapper other) {
        m_value += other.m_value;
        return *this;
      }

      DoubleWrapper operator +(DoubleWrapper other) const {
        auto copy = *this;
        copy += other;
        return copy;
      }

      bool operator ==(DoubleWrapper other) const {
        return m_value == other.m_value;
      }

      bool operator !=(DoubleWrapper other) const {
        return !(*this == other);
      }

      bool operator <(DoubleWrapper other) const {
        return m_value < other.m_value;
      }

      DoubleWrapper operator -() const {
        auto copy = *this;
        copy.m_value = -copy.m_value;
        return copy;
      }

    private:
      double m_value;
  };

  template<int I>
  DoubleWrapper<I> operator *(double scalar, DoubleWrapper<I> value) {
    return scalar * static_cast<double>(value);
  }

  using TestSample = Sample<DoubleWrapper<0>, DoubleWrapper<1>,
    DoubleWrapper<2>>;
  using TestTrial = ListTrial<TestSample>;
}

TEST_CASE("test_model_unique_arguments", "[Model]") {
  SECTION("One sample.") {
    auto trial = TestTrial();
    trial.insert({ 2., { 5., 7. } });
    auto model = Model<TestAlgorithm, TestTrial>(trial);
    REQUIRE(model({ 1., 1. }) == Approx(2.));
    REQUIRE(model({ 10., 1. }) == Approx(2.));
    REQUIRE(model({ 1., 10. }) == Approx(2.));
    REQUIRE(model({ -5., -5. }) == Approx(2.));
    REQUIRE(model({ 0., 0. }) == Approx(2.));
    REQUIRE(model({ -100., 100. }) == Approx(2.));
  }
  SECTION("Two samples.") {
    auto trial = TestTrial();
    trial.insert({ 2., { 5., 7. } });
    trial.insert({ 4., { 3., 9. } });
    auto model = Model<TestAlgorithm, TestTrial>(trial);
    REQUIRE(model({ 1., 1. }) == Approx(3.));
    REQUIRE(model({ 10., 1. }) == Approx(3.));
    REQUIRE(model({ 1., 10. }) == Approx(3.));
    REQUIRE(model({ -5., -5. }) == Approx(3.));
    REQUIRE(model({ 0., 0. }) == Approx(3.));
    REQUIRE(model({ -100., 100. }) == Approx(3.));
  }
  SECTION("Three samples.") {
    auto trial = TestTrial();

    // Normalized to { 1.0, 0.6 }.
    trial.insert({ 2., { 5., 7. } });

    // Normalized to { 0.5, 1.0 }.
    trial.insert({ 4., { 3., 9. } });

    // Normalized to { 0.0, 0.0 }.
    trial.insert({ 5., { 1., 4. } });

    auto model = Model<TestAlgorithm, TestTrial>(trial);
    REQUIRE(model({ 1., 4. }) == Approx(4.5));
    REQUIRE(model({ 0., 4. }) == Approx(4.5));
    REQUIRE(model({ -5., 4. }) == Approx(4.5));
    REQUIRE(model({ 3., 9. }) == Approx(3.));
    REQUIRE(model({ 4., 9. }) == Approx(3.));
    REQUIRE(model({ 5., 9. }) == Approx(3.));
    REQUIRE(model({ 4., 8. }) == Approx(3.));
    REQUIRE(model({ 1., 0. }) == Approx(3.5));
  }
}

TEST_CASE("test_model_singularity_resolution", "[Model]") {
  SECTION("Two samples.") {
    auto trial = TestTrial();
    trial.insert({ 2., { 0., 0. } });
    trial.insert({ 4., { 3., 9. } });
    auto model = Model<TestAlgorithm, TestTrial>(trial);
    REQUIRE(model({ 0., 0. }) == Approx(3.));
    REQUIRE(model({ 10., 1. }) == Approx(3.));
    REQUIRE(model({ 1., 10. }) == Approx(3.));
    REQUIRE(model({ -5., -5. }) == Approx(3.));
    REQUIRE(model({ 0., 0. }) == Approx(3.));
    REQUIRE(model({ -100., 100. }) == Approx(3.));
  }
  SECTION("Three samples.") {
    auto trial = TestTrial();
    trial.insert({ 2., { 0., 0. } });
    trial.insert({ 4., { 3., 9. } });
    trial.insert({ 2., { 0., 0. } });
    auto model = Model<TestAlgorithm, TestTrial>(trial);
    REQUIRE(model({ -100., -100. }) == Approx(2.));
    REQUIRE(model({ 0., 0. }) == Approx(2.));
    REQUIRE(model({ 2., 0. }) == Approx(2.));
    REQUIRE(model({ 0., 2. }) == Approx(2.));
    REQUIRE(model({ 1., 2. }) == Approx(2.));
    REQUIRE(model({ 1., 2. }) == Approx(2.));
    REQUIRE(model({ 3., 9. }) == Approx(3.));
    REQUIRE(model({ 2., 7. }) == Approx(3.));
    REQUIRE(model({ 4., 11. }) == Approx(3.));
    REQUIRE(model({ 100., 100. }) == Approx(3.));
  }
}

TEST_CASE("test_model_reuse_for_other_types", "[Model]") {
  using OtherSample = Sample<std::chrono::duration<double>, double,
    std::chrono::duration<double>>;
  using OtherTrial = ListTrial<OtherSample>;
  auto trial = OtherTrial();
  // Normalized to { 1.0, 0.6 }.
  trial.insert({ 2.s, { 5., 7.min } });
  // Normalized to { 0.5, 1.0 }.
  trial.insert({ 4.s, { 3., 9.min } });
  // Normalized to { 0.0, 0.0 }.
  trial.insert({ 6.s, { 1., 4.min } });
  auto model = Model<TestAlgorithm, OtherTrial>(trial);
  REQUIRE(model({ 1., 4.min }) == 5.s);
  REQUIRE(model({ 0., 4.min }) == 5.s);
  REQUIRE(model({ -5., 4.min }) == 5.s);
  REQUIRE(model({ 3., 9.min }) == 3.s);
  REQUIRE(model({ 4., 9.min }) == 3.s);
  REQUIRE(model({ 5., 9.min }) == 3.s);
  REQUIRE(model({ 4., 8.min }) == 3.s);
  REQUIRE(model({ 1., 0.min }) == 4.s);
}
