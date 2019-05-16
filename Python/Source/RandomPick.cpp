#include "Rover/Evaluator.hpp"
#include "Rover/Range.hpp"
#include "Autobox.hpp"
#include "Pick.hpp"
#include "RandomPick.hpp"

using namespace pybind11;
using namespace Rover;

namespace {
  class PythonRandomPick {
    public:
      using Type = object;

      PythonRandomPick(args generators)
        : m_pick(Box<std::size_t>(Range<std::size_t, std::size_t>(0,
            generators.size() - 1)), std::move(generators)) {}

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
