#include <catch.hpp>
#include "Rover/ListTrial.hpp"
#include "Rover/Sample.hpp"

using namespace Rover;

TEST_CASE("test_list_trial_size_and_capacity", "[ListTrial]") {
  SECTION("Insert samples.") {
    auto t = ListTrial<Sample<int, double, char>>();
    REQUIRE(t.size() == 0);
    t.insert({ 1, { 0.5, 'a' } });
    REQUIRE(t.size() == 1);
    REQUIRE(t.capacity() >= 1);
    t.insert({ 1, { 0.5, 'a' } });
    REQUIRE(t.size() == 2);
    REQUIRE(t.capacity() >= 2);
    t.insert({ 5, { 1.5, 'c' } });
    REQUIRE(t.size() == 3);
    REQUIRE(t.capacity() >= 3);
  }
  SECTION("Insert by iterators.") {
    auto t = ListTrial<Sample<int, double, char>>();
    auto v = std::vector<Sample<int, double, char>> {
      { 1, { 0.5, 'a' } }, { 5, { 1.5, 'c' } }
    };
    t.insert(v.begin(), v.end());
    REQUIRE(t.size() == 2);
    REQUIRE(t.capacity() >= 2);
    auto t2 = ListTrial<Sample<int, double, char>>();
    t2.insert({ 4, { -0.5, 'b' } });
    t2.insert({ 3, { 1.0, 'd' } });
    t.insert(t2.begin(), t2.end());
    REQUIRE(t.size() == 4);
    REQUIRE(t.capacity() >= 4);
  }
  SECTION("Reserve") {
    auto t = ListTrial<Sample<int, double, char>>();
    t.reserve(5);
    REQUIRE(t.capacity() >= 5);
    t.reserve(100);
    REQUIRE(t.capacity() >= 100);
    t.reserve(5000);
    REQUIRE(t.capacity() >= 5000);
  }
}

TEST_CASE("test_list_trial_values", "[ListTrial]") {
  SECTION("Insert samples.") {
    auto t = ListTrial<Sample<int, double, char>>();
    t.insert({ 1, { 0.5, 'a' } });
    t.insert({ 1, { 0.5, 'a' } });
    t.insert({ 5, { 1.5, 'c' } });
    REQUIRE(std::get<0>(t[0].m_arguments) == 0.5);
    REQUIRE(std::get<1>(t[0].m_arguments) == 'a');
    REQUIRE(t[0].m_result == 1);
    REQUIRE(std::get<0>(t[1].m_arguments) == 0.5);
    REQUIRE(std::get<1>(t[1].m_arguments) == 'a');
    REQUIRE(t[1].m_result == 1);
    REQUIRE(std::get<0>(t[2].m_arguments) == 1.5);
    REQUIRE(std::get<1>(t[2].m_arguments) == 'c');
    REQUIRE(t[2].m_result == 5);
  }
  SECTION("Insert by iterators.") {
    auto t = ListTrial<Sample<int, double, char>>();
    auto v = std::vector<Sample<int, double, char>>{
      { 1, { 0.5, 'a' } }, { 5, { 1.5, 'c' } }
    };
    t.insert(v.begin(), v.end());
    auto t2 = ListTrial<Sample<int, double, char>>();
    t2.insert({ 4, { -0.5, 'b' } });
    t2.insert({ 3, { 1.0, 'd' } });
    t.insert(t2.begin(), t2.end());
    REQUIRE(std::get<0>(t[0].m_arguments) == 0.5);
    REQUIRE(std::get<1>(t[0].m_arguments) == 'a');
    REQUIRE(t[0].m_result == 1);
    REQUIRE(std::get<0>(t[1].m_arguments) == 1.5);
    REQUIRE(std::get<1>(t[1].m_arguments) == 'c');
    REQUIRE(t[1].m_result == 5);
    REQUIRE(std::get<0>(t[2].m_arguments) == -0.5);
    REQUIRE(std::get<1>(t[2].m_arguments) == 'b');
    REQUIRE(t[2].m_result == 4);
    REQUIRE(std::get<0>(t[3].m_arguments) == 1.0);
    REQUIRE(std::get<1>(t[3].m_arguments) == 'd');
    REQUIRE(t[3].m_result == 3);
  }
}
