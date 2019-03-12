#include <type_traits>
#include <variant>
#include "Range.hpp"

using namespace pybind11;
using namespace Rover;

namespace {
  class PythonRange {
    public:
      using Type = Range<object, object>::Type;

      PythonRange(object begin, object end)
        : m_impl(Range(std::move(begin), std::move(end))) {}

      PythonRange(object begin, object end, object granularity)
        : m_impl(Range(std::move(begin), std::move(end),
            std::move(granularity))) {}

      Type generate(Evaluator& e) {
        return std::visit([&] (auto& impl) {
          return impl.generate(e);
        }, m_impl);
      }

    private:
      using ContinuousRange = Range<object, object>;
      using DiscreteRange = Range<object, object, object>;
      std::variant<ContinuousRange, DiscreteRange> m_impl;
  };
}
void Rover::export_range(module& module) {
  class_<PythonRange>(module, "Range")
    .def(init<object, object>())
    .def(init<object, object, object>())
    .def("generate", &PythonRange::generate);

  implicitly_convertible<PythonRange, Box<PythonRange::Type>>();
}
