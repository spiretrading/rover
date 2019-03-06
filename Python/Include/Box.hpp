#ifndef ROVER_PYTHON_BOX_HPP
#define ROVER_PYTHON_BOX_HPP
#include <optional>
#include <string_view>
#include <pybind11/pybind11.h>
#include "Rover/Constant.hpp"
#include "Rover/Box.hpp"

namespace pybind11::detail {
  template<>
  struct type_caster<Rover::Box<object>> {
    std::optional<Rover::Box<object>> value;

    static PYBIND11_DESCR name() {
      return type_descr(_("Box"));
    }

    template<typename T_,
      std::enable_if_t<std::is_same_v<Rover::Box<object>, std::remove_cv_t<T_>>,
      int> = 0>
    static handle cast(T_* src, return_value_policy policy, handle parent) {
      if(!src) {
        return none().release();
      }
      if(policy == return_value_policy::take_ownership) {
        auto h = cast(std::move(*src), policy, parent);
        delete src;
        return h;
      } else {
        return cast(*src, policy, parent);
      }
    }

    operator Rover::Box<object>* () {
      return &*value;
    }

    operator Rover::Box<object>& () {
      return *value;
    }

    operator Rover::Box<object>&& () && {
      return std::move(*value);
    }

    template <typename T_> using cast_op_type =
      pybind11::detail::movable_cast_op_type<T_>;

    bool load(handle src, bool) {
      auto source = src.ptr();
      if(!isinstance<Rover::Constant<object>>(src)) {
        return false;
      }
      value.reset();
      value.emplace(src.cast<Rover::Constant<object>>());
      return true;
    }

    static handle cast(Rover::Box<object> src, return_value_policy, handle) {
      return Py_None;
    }
  };
}

namespace Rover {

  //! Exports an instantiation of the Box class.
  /*!
    \param module The module to export the class to.
    \param type_name The name of the type the Box generates.
  */
  template<typename T>
  void export_box(pybind11::module& module, std::string_view type_name);

  //! Exports a generic Box for Python objects.
  void export_box(pybind11::module& module);

  template<typename T>
  void export_box(pybind11::module& module, std::string_view type_name) {
    auto name = std::string("Box").append(type_name);
    pybind11::class_<Box<T>>(module, name.c_str())
      .def("generate", &Box<T>::generate);
  }
}

#endif
