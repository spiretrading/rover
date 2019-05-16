#include "Rover/Evaluator.hpp"
#include "Rover/Range.hpp"
#include "Pick.hpp"
#include "RandomPick.hpp"

using namespace pybind11;
using namespace Rover;

namespace {
  class PythonRandomPick {
    public:
      class Choice {
        public:
          explicit Choice(std::size_t num_generators)
            : m_range(0, num_generators - 1) {}

          std::size_t operator ()() {
            auto evaluator = Evaluator();
            return m_range.generate(evaluator);
          }

        private:
          Range<std::size_t, std::size_t> m_range;
      };
      
      using Type = object;

      PythonRandomPick(args generators)
        : m_pick(cast(Choice(generators.size())), std::move(generators)) {}

      Type generate(Evaluator& evaluator) {
        return m_pick.generate(evaluator);
      }

    private:
      PythonPick m_pick;
  };
}

void Rover::export_random_pick(module& module) {
  class_<PythonRandomPick::Choice>(module, "Choice")
    .def(init<std::size_t>())
    .def("__call__", &PythonRandomPick::Choice::operator ());
  class_<PythonRandomPick>(module, "RandomPick")
    .def(init<args>())
    .def("generate", &PythonRandomPick::generate);
  implicitly_convertible<PythonRandomPick, Box<object>>();
}
