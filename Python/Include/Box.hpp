#ifndef ROVER_PYTHON_BOX_HPP
#define ROVER_PYTHON_BOX_HPP
#include <memory>
#include <string_view>
#include <pybind11/pybind11.h>
#include "Rover/Box.hpp"
#include "Rover/Constant.hpp"
#include "Rover/Evaluator.hpp"
#include "Rover/Noncopyable.hpp"

namespace Rover::Details {
  template<typename T>
  class PythonBox : private Noncopyable {
    public:
      using Type = T;

      PythonBox(pybind11::object obj)
        : m_obj(std::move(obj)) {
      }

      Type generate(Evaluator& e) {
        auto wrapper = std::shared_ptr<Evaluator>(&e, [](auto ptr) {});
        return m_obj.attr("generate")(std::move(wrapper)).cast<Type>();
      }

    private:
      pybind11::object m_obj;
  };

  bool is_python_generator(const pybind11::object& arg);
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
      .def(init(
        [](pybind11::object arg) {
          if(Details::is_python_generator(arg)) {
            return std::make_unique<Box<T>>(Details::PythonBox<T>(std::move(
              arg)));
          } else {
            return std::make_unique<Box<T>>(Constant(arg.cast<T>()));
          }
        }))
      .def("generate", &Box<T>::generate);
    if constexpr(!std::is_same_v<T, pybind11::object>) {
      pybind11::implicitly_convertible<Box<T>, Box<pybind11::object>>();
      pybind11::implicitly_convertible<Box<pybind11::object>, Box<T>>();
    }
  }
}

#endif
