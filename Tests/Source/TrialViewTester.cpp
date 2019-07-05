#include <catch2/catch.hpp>
#include "Rover/ListTrial.hpp"
#include "Rover/Sample.hpp"
#include "Rover/TrialView.hpp"

using namespace Rover;

TEST_CASE("test_list_trial_to_view", "[TrialView]") {
  SECTION("Empty trial.") {
    auto t = ListTrial<Sample<int, double, char>>();
    auto v = TrialView(t);
    REQUIRE(v.size() == 0);
    REQUIRE(v.begin() == v.end());
  }
  SECTION("One sample.") {
    auto t = ListTrial<Sample<int, double, char>>();
    t.insert({ 1, { 0.5, 'c' } });
    auto v = TrialView(t);
    REQUIRE(v.size() == 1);
    REQUIRE(v.begin() != v.end());
    auto b = v.begin();
    REQUIRE(b->m_result == 1);
    REQUIRE(std::get<0>(b->m_arguments) == 0.5);
    REQUIRE(std::get<1>(b->m_arguments) == 'c');
    const auto& s1 = v[0];
    REQUIRE(s1.m_result == 1);
    REQUIRE(std::get<0>(s1.m_arguments) == 0.5);
    REQUIRE(std::get<1>(s1.m_arguments) == 'c');
    const auto& s2 = *b;
    REQUIRE(s2.m_result == 1);
    REQUIRE(std::get<0>(s2.m_arguments) == 0.5);
    REQUIRE(std::get<1>(s2.m_arguments) == 'c');
  }
  SECTION("Many samples.") {
    auto t = ListTrial<Sample<int, double, char>>();
    t.insert({ 1, { 0.5, 'c' } });
    t.insert({ 6, { 0.0, 'a' } });
    t.insert({ 5, { 0.3, 'd' } });
    auto v = TrialView(t);
    REQUIRE(v.size() == 3);
    auto count = 0;
    for(const auto& s : v) {
      ++count;
    }
    REQUIRE(count == 3);
    const auto& s1 = v[0];
    REQUIRE(s1.m_result == 1);
    REQUIRE(std::get<0>(s1.m_arguments) == 0.5);
    REQUIRE(std::get<1>(s1.m_arguments) == 'c');
    auto s2 = v[1];
    REQUIRE(s2.m_result == 6);
    REQUIRE(std::get<0>(s2.m_arguments) == 0.0);
    REQUIRE(std::get<1>(s2.m_arguments) == 'a');
    auto s3 = v[2];
    REQUIRE(s3.m_result == 5);
    REQUIRE(std::get<0>(s3.m_arguments) == 0.3);
    REQUIRE(std::get<1>(s3.m_arguments) == 'd');
  }
  SECTION("Random access by iterator.") {
    auto t = ListTrial<Sample<int, double, char>>();
    t.insert({ 1, { 0.5, 'c' } });
    t.insert({ 6, { 0.0, 'a' } });
    t.insert({ 5, { 0.3, 'd' } });
    auto v = TrialView(t);
    auto it = v.begin();
    REQUIRE(it == v.begin());
    REQUIRE(it != v.end());
    REQUIRE(it < v.end());
    REQUIRE(it <= v.end());
    {
      const auto& s = *it;
      REQUIRE(s.m_result == 1);
      REQUIRE(std::get<0>(s.m_arguments) == 0.5);
      REQUIRE(std::get<1>(s.m_arguments) == 'c');
    }
    {
      const auto& s = it[0];
      REQUIRE(s.m_result == 1);
      REQUIRE(std::get<0>(s.m_arguments) == 0.5);
      REQUIRE(std::get<1>(s.m_arguments) == 'c');
    }
    {
      const auto& s = it[2];
      REQUIRE(s.m_result == 5);
      REQUIRE(std::get<0>(s.m_arguments) == 0.3);
      REQUIRE(std::get<1>(s.m_arguments) == 'd');
    }
    it += 2;
    REQUIRE(it != v.end());
    REQUIRE(it < v.end());
    REQUIRE(it <= v.end());
    REQUIRE(it != v.begin());
    REQUIRE(it > v.begin());
    REQUIRE(it >= v.begin());
    {
      const auto& s = *it;
      REQUIRE(s.m_result == 5);
      REQUIRE(std::get<0>(s.m_arguments) == 0.3);
      REQUIRE(std::get<1>(s.m_arguments) == 'd');
    }
    --it;
    {
      const auto& s = *it;
      REQUIRE(s.m_result == 6);
      REQUIRE(std::get<0>(s.m_arguments) == 0.0);
      REQUIRE(std::get<1>(s.m_arguments) == 'a');
    }
    it = it + 2;
    REQUIRE(it == v.end());
  }
}
