#include <string>
#include <catch.hpp>
#include "Rover/Constant.hpp"
#include "Rover/Generator.hpp"
#include "Rover/Range.hpp"
#include "Rover/Reference.hpp"

using namespace Rover;
using namespace std::string_literals;

TEST_CASE("test_open_int_range", "[Range]") {
  auto range = Range(Constant(1), Constant(10), Interval::OPEN);
  auto ref = Reference(range);
//  REQUIRE(range.generate() == ref.generate() ??);
}
