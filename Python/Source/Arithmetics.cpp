#include "Arithmetics.hpp"

namespace Rover::Details {
  const pybind11::module& import_operator() {
    static auto module = pybind11::module::import("operator");
    return module;
  }
}

namespace pybind11 {
  object abs(const object& obj) {
    return module::import("builtins").attr("abs")(obj);
  }

  object floor(const object& obj) {
    return module::import("math").attr("floor")(obj);
  }
}
