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

      PythonBox(pybind11::object obj)
        : m_obj(std::move(obj)) {}

      bool is_same(const PythonBox<T>& right) const {
        return m_obj.is(right.m_obj);
      }

      Type generate(Evaluator& e) {
        auto evaluator_ptr = std::shared_ptr<Evaluator>(&e, [](auto) {});
        auto evaluator_handle = pybind11::cast(evaluator_ptr).release();
        auto evaluator = pybind11::reinterpret_borrow<pybind11::object>(
          evaluator_handle);
        return m_obj.attr("generate")(std::move(evaluator)).cast<Type>();
      }

    private:
      pybind11::object m_obj;
  };

  bool is_python_generator(const pybind11::object& arg);
}

namespace Rover {
  template<typename T>
  bool is_same(const Details::PythonBox<T>& left,
      const Details::PythonBox<T>& right) {
    return left.is_same(right);
  }

  template<typename T, typename ArgFwd>
  Box<T> python_autobox(ArgFwd&& arg) {
    if(Details::is_python_generator(arg)) {
      return Box(Details::PythonBox<T>(std::forward<ArgFwd>(arg)));
    } else {
      return Box(Constant(arg.template cast<T>()));
    }
  }
}

#endif
