#include "Rover/Python/RandomPick.hpp"
#include "Rover/Evaluator.hpp"
#include "Rover/Range.hpp"
#include "Rover/Python/Autobox.hpp"
#include "Rover/Python/Pick.hpp"

using namespace pybind11;
using namespace Rover;

namespace {
  class PythonRandomPick {
    public:
      using Type = object;

      PythonRandomPick(args generators)
        : m_pick(Box<int>(Range(0, static_cast<int>(generators.size()) - 1)),
            std::move(generators)) {}

      Type generate(Evaluator& evaluator) {
        return m_pick.generate(evaluator);
      }

    private:
      PythonPick m_pick;
  };
}

void Rover::export_random_pick(module& module) {
  class_<PythonRandomPick>(module, "RandomPick")
    .def(init<args>())
    .def("generate", &PythonRandomPick::generate);
  implicitly_convertible<PythonRandomPick, Box<object>>();
}
