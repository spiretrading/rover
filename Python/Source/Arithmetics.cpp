#include "Arithmetics.hpp"

namespace {
  const pybind11::module& import_builtins() {
    static auto module = pybind11::module::import("builtins");
    return module;
  }

  const pybind11::module& import_math() {
    static auto module = pybind11::module::import("math");
    return module;
  }
}

namespace Rover::Details {
  const pybind11::module& import_operator() {
    static auto module = pybind11::module::import("operator");
    return module;
  }
}

namespace pybind11 {
  object abs(const object& obj) {
    return import_builtins().attr("abs")(obj);
  }

  object floor(const object& obj) {
    return import_math().attr("floor")(obj);
  }
}
