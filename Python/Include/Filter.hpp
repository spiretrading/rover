#ifndef ROVER_PYTHON_FILTER_HPP
#define ROVER_PYTHON_FILTER_HPP
#include <functional>
#include <string_view>
#include <pybind11/pybind11.h>
#include "Rover/Filter.hpp"
#include "Box.hpp"

namespace Rover {
 
  //! Exports Filter<std::function<bool (pybind11::object)>,
  //! Box<pybind11::object>> as the canonical Filter generator.
  /*!
    \param module The module to export the generator to.
  */
  void export_filter(pybind11::module& module);

  //! Exports the Filter generator for a given generator type.
  /*!
    \tparam G The type of the generator.
    \param module The module to export the generator to.
    \param generator_name The name of the generator, which becomes a suffix
                          to the name of the exported Filter.
  */
  template<typename G>
  void export_filter(pybind11::module& module,
    std::string_view generator_name);

  template<typename G>
  void export_filter(pybind11::module& module,
      std::string_view generator_name) {
    using ValueType = typename G::Type;
    using FilterType = Filter<std::function<bool (const ValueType&)>, G>;
    auto name = std::string("Filter").append(generator_name);
    pybind11::class_<FilterType>(module, name.c_str())
      .def(pybind11::init([](pybind11::object predicate, G generator) {
         return FilterType([predicate = std::move(predicate)](
               const ValueType& value) {
             return predicate.attr("__call__")(
               pybind11::cast(value)).cast<bool>();
           }, std::move(generator));
       }))
      .def("generate", &FilterType::generate);
    pybind11::implicitly_convertible<FilterType,
      Box<typename FilterType::Type>>();
    pybind11::implicitly_convertible<FilterType, Box<pybind11::object>>();
    pybind11::implicitly_convertible<FilterType,
      Filter<std::function<bool (pybind11::object)>, Box<pybind11::object>>>();
  }
}

#endif
