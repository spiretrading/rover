#include "Rover/Evaluator.hpp"
#include "Rover/Range.hpp"
#include "Pick.hpp"
#include "RandomPick.hpp"

using namespace pybind11;
using namespace Rover;

namespace {
  class IntegerRange : public Range<std::size_t, std::size_t> {
    public:
      using Range::Type;
      using Range::Range;
  };

  class PythonRandomPick {
    public:
      using Type = object;

      PythonRandomPick(args generators)
        : m_pick(cast(IntegerRange(0, generators.size() - 1)),
            std::move(generators)) {}

      Type generate(Evaluator& evaluator) {
        return m_pick.generate(evaluator);
      }

    private:
      PythonPick m_pick;
  };
}

void Rover::export_random_pick(module& module) {
  class_<IntegerRange>(module, "_IntegerRange")
    .def("generate", &IntegerRange::generate);
  class_<PythonRandomPick>(module, "RandomPick")
    .def(init<args>())
    .def("generate", &PythonRandomPick::generate);
  implicitly_convertible<PythonRandomPick, Box<object>>();
}
