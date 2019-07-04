#ifndef ROVER_PYTHON_RANGE_HPP
#define ROVER_PYTHON_RANGE_HPP
#include <memory>
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
        using ContinuousRange = Range<Box<pybind11::object>,
          Box<pybind11::object>>;
        using DiscreteRange = Range<Box<pybind11::object>,
          Box<pybind11::object>, Box<pybind11::object>>;
        
        PythonRange(ContinuousRange impl);

        PythonRange(DiscreteRange impl);

        PythonRange(pybind11::object begin, pybind11::object end);

        PythonRange(pybind11::object begin, pybind11::object end,
          Interval interval);

        PythonRange(pybind11::object begin, pybind11::object end,
          pybind11::object granularity);

        PythonRange(pybind11::object begin, pybind11::object end,
          pybind11::object granularity, Interval interval);

        Type generate(Evaluator& e);

      private:
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
    static_assert(!std::is_same_v<Begin, pybind11::object> &&
      !std::is_same_v<End, pybind11::object> &&
      !std::is_same_v<Granularity, pybind11::object>);
    auto name = std::string("Range").append(type_name);
    if constexpr(std::is_same_v<Granularity, void>) {
      pybind11::class_<Range<Begin, End>>(module, name.c_str())
        .def(pybind11::init<Begin, End>())
        .def(pybind11::init<Begin, End, Interval>())
        .def("generate", &Range<Begin, End, Granularity>::generate);
    } else {
      pybind11::class_<Range<Begin, End, Granularity>>(module, name.c_str())
        .def(pybind11::init<Begin, End, Granularity>())
        .def(pybind11::init<Begin, End, Granularity, Interval>())
        .def("generate", &Range<Begin, End, Granularity>::generate);
    }
    pybind11::implicitly_convertible<Range<Begin, End, Granularity>,
      Details::PythonRange>();
    pybind11::implicitly_convertible<Range<Begin, End, Granularity>,
      Box<typename Range<Begin, End, Granularity>::Type>>();
    pybind11::implicitly_convertible<Range<Begin, End, Granularity>,
      Box<pybind11::object>>();
  }
}

#endif
