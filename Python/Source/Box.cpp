#include "Box.hpp"

using namespace pybind11;
using namespace Rover;

template<>
void Rover::export_box<object>(module& module) {
}
