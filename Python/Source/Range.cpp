#include "Autobox.hpp"
#include "Range.hpp"

using namespace pybind11;
using namespace Rover;

namespace Rover::Details {
  PythonRange::PythonRange(ContinuousRange impl)
    : m_impl(std::move(impl)) {}

  PythonRange::PythonRange(DiscreteRange impl)
    : m_impl(std::move(impl)) {}

  PythonRange::PythonRange(object begin, object end)
    : m_impl(Range(python_autobox<object>(std::move(begin)),
        python_autobox<object>(std::move(end)))) {}

  PythonRange::PythonRange(object begin, object end, Interval interval)
    : m_impl(Range(python_autobox<object>(std::move(begin)),
        python_autobox<object>(std::move(end)), interval)) {}

  PythonRange::PythonRange(object begin, object end, object granularity)
    : m_impl(Range(python_autobox<object>(std::move(begin)),
        python_autobox<object>(std::move(end)),
        python_autobox<object>(std::move(granularity)))) {}

  PythonRange::PythonRange(object begin, object end, object granularity,
      Interval interval)
    : m_impl(Range(python_autobox<object>(std::move(begin)),
        python_autobox<object>(std::move(end)),
        python_autobox<object>(std::move(granularity)),
        interval)) {}

  PythonRange::Type PythonRange::generate(Evaluator& e) {
    return std::visit([&](auto& impl) {
      return impl.generate(e);
    }, m_impl);
  }
}

namespace Rover {
  void export_range(module& module) {
    enum_<Interval>(module, "Interval")
      .value("CLOSED", Interval::CLOSED)
      .value("LEFT_EXCLUSIVE", Interval::LEFT_EXCLUSIVE)
      .value("RIGHT_EXCLUSIVE", Interval::RIGHT_EXCLUSIVE)
      .value("OPEN", Interval::OPEN)
      .export_values();

    class_<Details::PythonRange>(module, "Range")
      .def(init<object, object>())
      .def(init<object, object, Interval>())
      .def(init<object, object, object>())
      .def(init<object, object, object, Interval>())
      .def("generate", &Details::PythonRange::generate);

    implicitly_convertible<Details::PythonRange, Box<object>>();
  }
}
