#ifndef ROVER_PYTHON_AUTOBOX_HPP
#define ROVER_PYTHON_AUTOBOX_HPP
#include <memory>
#include <type_traits>
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

      template<typename ObjFwd, std::enable_if_t<!std::is_convertible_v<ObjFwd,
        PythonBox>>* = nullptr>
      PythonBox(ObjFwd&& obj)
        : m_obj(std::forward<ObjFwd>(obj)) {}

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
  template<typename T, typename ArgFwd>
  auto python_autobox(ArgFwd&& arg) {
    if(Details::is_python_generator(arg)) {
      return std::make_unique<Box<T>>(Details::PythonBox<T>(
        std::forward<ArgFwd>(arg)));
    } else {
      return std::make_unique<Box<T>>(Constant(arg.cast<T>()));
    }
  }
}

#endif
