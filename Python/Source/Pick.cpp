#include <pybind11/functional.h>
#include "Autobox.hpp"
#include "Pick.hpp"

using namespace pybind11;
using namespace Rover;

namespace Rover {
  void export_pick(module& module) {
    class_<PythonPick>(module, "Pick")
      .def(init<object, args>())
      .def("generate", &PythonPick::generate);
    implicitly_convertible<PythonPick, Box<object>>();
  }

  PythonPick::PythonPick(Box<std::size_t> choice, args generators)
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
    : PythonPick(python_autobox<std::size_t, object>(std::move(choice)),
        std::move(generators)) {}

  PythonPick::Type PythonPick::generate(Evaluator& evaluator) {
    auto index = evaluator.evaluate(m_choice);
    auto value = evaluator.evaluate(m_generators[index]);
    return value;
  }
}
