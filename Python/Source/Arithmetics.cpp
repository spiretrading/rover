#include "Rover/Python/Arithmetics.hpp"

using namespace pybind11;
using namespace Rover;
using namespace Rover::Details;

namespace {
  const module& import_builtins() {
    static auto module = module::import("builtins");
    return module;
  }

  const module& import_math() {
    static auto module = module::import("math");
    return module;
  }
}

object pybind11::operator -(const object& obj) {
  return apply_operator("__neg__", obj);
}

object pybind11::abs(const object& obj) {
  return import_builtins().attr("abs")(obj);
}

object pybind11::floor(const object& obj) {
  return import_math().attr("floor")(obj);
}

const pybind11::module& Rover::Details::import_operator() {
  static auto module = module::import("operator");
  return module;
}
