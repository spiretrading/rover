#include "Rover/Python/Pick.hpp"
#include <pybind11/functional.h>
#include "Rover/Python/Autobox.hpp"

using namespace pybind11;
using namespace Rover;

PythonPick::PythonPick(Box<int> choice, args generators)
  : m_choice(std::move(choice)),
    m_generators([&] {
      auto result = std::vector<Box<object>>();
      for(auto i = std::size_t(0); i < generators.size(); ++i) {
        result.push_back(python_autobox<object, object>(std::move(
          generators[i])));
      }
      return result;
    }()) {}

PythonPick::PythonPick(object choice, args generators)
  : PythonPick(python_autobox<int, object>(std::move(choice)),
      std::move(generators)) {}

PythonPick::Type PythonPick::generate(Evaluator& evaluator) {
  auto index = evaluator.evaluate(m_choice);
  auto value = evaluator.evaluate(m_generators[index]);
  return value;
}

void Rover::export_pick(module& module) {
  class_<PythonPick>(module, "Pick")
    .def(init<object, args>())
    .def("generate", &PythonPick::generate);
  implicitly_convertible<PythonPick, Box<object>>();
}
