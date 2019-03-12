#include "Range.hpp"

using namespace pybind11;
using namespace Rover;

namespace Rover::Details {
  template<typename ImplFwd, std::enable_if_t<std::is_convertible_v<
    std::decay_t<ImplFwd>, PythonRange::ContinuousRange>>*>
  PythonRange::PythonRange(ImplFwd&& impl)
    : m_impl(std::forward<ImplFwd>(impl)) {}

  template<typename ImplFwd, std::enable_if_t<std::is_convertible_v<
    std::decay_t<ImplFwd>, PythonRange::DiscreteRange>>*>
  PythonRange::PythonRange(ImplFwd&& impl)
    : m_impl(std::forward<ImplFwd>(impl)) {}

  PythonRange::PythonRange(object begin, object end)
    : m_impl(Range(std::move(begin), std::move(end))) {}

  PythonRange::PythonRange(object begin, object end, Interval interval)
    : m_impl(Range(std::move(begin), std::move(end), interval)) {}

  PythonRange::PythonRange(object begin, object end, object granularity)
    : m_impl(Range(std::move(begin), std::move(end),
        std::move(granularity))) {}

  PythonRange::PythonRange(object begin, object end, object granularity,
      Interval interval)
    : m_impl(Range(std::move(begin), std::move(end), std::move(granularity),
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
