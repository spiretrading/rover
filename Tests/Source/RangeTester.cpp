#include <string>
#include <catch.hpp>
#include "Rover/Constant.hpp"
#include "Rover/Generator.hpp"
#include "Rover/Range.hpp"

using namespace Rover;

TEST_CASE("test_open_int_range", "[Range]") {
  auto range = Range(Constant(1), Constant(10), Interval::OPEN);
}
