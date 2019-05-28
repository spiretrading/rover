#include <sstream>
#include <catch.hpp>
#include "Rover/CsvParser.hpp"
#include "Rover/ListTrial.hpp"
#include "Rover/Sample.hpp"

using namespace Rover;

TEST_CASE("test_trial_serialization", "[TrialCodec]") {
  SECTION("Empty trial.") {
    auto trial = ListTrial<Sample<int, int, std::string>>();
    auto stream = std::ostringstream();
    save_to_csv(trial, stream);
    REQUIRE(stream.str() == "");
  }
  SECTION("One sample.") {
    auto trial = ListTrial<Sample<int, int, std::string>>();
    trial.insert({ 5, { 7, "abc" } });
    auto stream = std::ostringstream();
    save_to_csv(trial, stream);
    REQUIRE(stream.str() == "5,7,abc\n");
  }
  SECTION("Multiple samples.") {
    auto trial = ListTrial<Sample<int, int, std::string>>();
    trial.insert({ 5, { 7, "abc" } });
    trial.insert({ -1, { 3, "abcde" } });
    trial.insert({ 0, { 6, "" } });
    auto stream = std::ostringstream();
    save_to_csv(trial, stream);
    REQUIRE(stream.str() == "5,7,abc\n-1,3,abcde\n0,6,\n");
  }
  SECTION("Special characters.") {
    auto trial = ListTrial<Sample<std::string, int, std::string>>();
    trial.insert({ ",a,", { 5, "\",\"," } });
    trial.insert({ ",\",", { 75, "\"abc\",\"qwe\""} });
    auto stream = std::ostringstream();
    save_to_csv(trial, stream);
    REQUIRE(stream.str() ==
      "\",a,\",5,\"\"\",\"\",\"\n\",\"\",\",75,\"\"\"abc\"\",\"\"qwe\"\"\"\n");
  }
}

TEST_CASE("test_trial_deserialization", "[TrialCodec]") {
  SECTION("Empty trial") {
    auto stream = std::istringstream("");
    auto trial = ListTrial<Sample<int, int, std::string>>();
    load_from_csv(stream, trial);
    REQUIRE(trial.size() == 0);
  }
  SECTION("One sample.") {
    auto stream = std::istringstream("5,7,abc\n");
    auto trial = ListTrial<Sample<int, int, std::string>>();
    load_from_csv(stream, trial);
    REQUIRE(trial.size() == 1);
    REQUIRE(trial[0].m_result == 5);
    REQUIRE(std::get<0>(trial[0].m_arguments) == 7);
    REQUIRE(std::get<1>(trial[0].m_arguments) == "abc");
  }
  SECTION("Multiple samples.") {
    auto stream = std::istringstream("5,7,abc\n-1,3,abcde\n0,6,\n");
    auto trial = ListTrial<Sample<int, int, std::string>>();
    load_from_csv(stream, trial);
    REQUIRE(trial.size() == 3);
    auto s1 = trial[0];
    REQUIRE(s1.m_result == 5);
    REQUIRE(std::get<0>(s1.m_arguments) == 7);
    REQUIRE(std::get<1>(s1.m_arguments) == "abc");
    auto s2 = trial[1];
    REQUIRE(s2.m_result == -1);
    REQUIRE(std::get<0>(s2.m_arguments) == 3);
    REQUIRE(std::get<1>(s2.m_arguments) == "abcde");
    auto s3 = trial[2];
    REQUIRE(s3.m_result == 0);
    REQUIRE(std::get<0>(s3.m_arguments) == 6);
    REQUIRE(std::get<1>(s3.m_arguments) == "");
  }
  SECTION("Special characters.") {
    auto stream = std::istringstream(
      "\",a,\",5,\"\"\",\"\",\"\n\",\"\",\",75,\"\"\"abc\"\",\"\"qwe\"\"\"\n");
    auto trial = ListTrial<Sample<std::string, int, std::string>>();
    load_from_csv(stream, trial);
    REQUIRE(trial.size() == 2);
    auto s1 = trial[0];
    REQUIRE(s1.m_result == ",a,");
    REQUIRE(std::get<0>(s1.m_arguments) == 5);
    REQUIRE(std::get<1>(s1.m_arguments) == "\",\",");
    auto s2 = trial[1];
    REQUIRE(s2.m_result == ",\",");
    REQUIRE(std::get<0>(s2.m_arguments) == 75);
    REQUIRE(std::get<1>(s2.m_arguments) == "\"abc\",\"qwe\"");
  }
}
