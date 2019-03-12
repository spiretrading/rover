#ifndef ROVER_PYTHON_RANGE_HPP
#define ROVER_PYTHON_RANGE_HPP
#include <string_view>
#include <type_traits>
#include <variant>
#include <pybind11/pybind11.h>
#include "Arithmetics.hpp"
#include "Rover/Box.hpp"
#include "Rover/Range.hpp"

namespace Rover {
  namespace Details {
    class PythonRange {
      public:
        using Type = Range<pybind11::object, pybind11::object>::Type;

        PythonRange(pybind11::object begin, pybind11::object end)
          : m_impl(Range(std::move(begin), std::move(end))) {}

        PythonRange(pybind11::object begin, pybind11::object end,
            pybind11::object granularity)
          : m_impl(Range(std::move(begin), std::move(end),
              std::move(granularity))) {}

        Type generate(Evaluator& e) {
          return std::visit([&] (auto& impl) {
            return impl.generate(e);
          }, m_impl);
        }

      private:
        using ContinuousRange = Range<pybind11::object, pybind11::object>;
        using DiscreteRange = Range<pybind11::object, pybind11::object,
          pybind11::object>;
        std::variant<ContinuousRange, DiscreteRange> m_impl;
    };
  }

  //! Exports an instantiation of the Range class.
  /*!
    \param module The module to export the class to.
    \param type_name The name of the type the Range generates.
  */
  template<typename B, typename E, typename G>
  void export_range(pybind11::module& module, std::string_view type_name);

  //! Exports a generic Range for Python objects.
  void export_range(pybind11::module& module);

  template<typename Begin, typename End, typename Granularity>
  void export_range(pybind11::module& module, std::string_view type_name) {
    auto name = std::string("Range").append(type_name);
    if constexpr(std::is_same_v<Granularity, void>) {
      pybind11::class_<Range<Begin, End>>(module, name.c_str())
        .def(pybind11::init<Begin, End>())
        .def("generate", &Range<Begin, End, Granularity>::generate);
    } else {
      pybind11::class_<Range<Begin, End, Granularity>>(module, name.c_str())
        .def(pybind11::init<Begin, End, Granularity>())
        .def("generate", &Range<Begin, End, Granularity>::generate);
    }
    pybind11::implicitly_convertible<Range<Begin, End, Granularity>,
      Box<typename Range<Begin, End, Granularity>::Type>>();
    if constexpr(!std::is_same_v<Begin, pybind11::object> ||
        !std::is_same_v<End, pybind11::object> || (
        !std::is_same_v<Granularity, pybind11::object> &&
        !std::is_same_v<Granularity, void>)) {
      if constexpr(std::is_same_v<Granularity, void>) {
        pybind11::implicitly_convertible<Range<Begin, End, void>,
          Range<pybind11::object, pybind11::object, void>>();
        pybind11::implicitly_convertible<Range<Begin, End, void>,
          Box<Range<pybind11::object, pybind11::object, void>::Type>>();
      } else {
        pybind11::implicitly_convertible<Range<Begin, End, Granularity>,
          Range<pybind11::object, pybind11::object, pybind11::object>>();
        pybind11::implicitly_convertible<Range<Begin, End, Granularity>,
          Box<Range<pybind11::object, pybind11::object,
          pybind11::object>::Type>>();
      }
    }
  }
}

#endif
