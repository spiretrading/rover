#include "Constant.hpp"

using namespace pybind11;
using namespace Rover;

template<>
void Rover::export_constant<object>(module& module) {
}
