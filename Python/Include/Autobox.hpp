#ifndef ROVER_PYTHON_AUTOBOX_HPP
#define ROVER_PYTHON_AUTOBOX_HPP
#include <memory>
#include <mutex>
#include <type_traits>
#include <vector>
#include <pybind11/pybind11.h>
#include "Rover/Box.hpp"
#include "Rover/Constant.hpp"
#include "Rover/Evaluator.hpp"
#include "Rover/Noncopyable.hpp"

namespace Rover::Details {
  class PythonBoxPointerPool : private Noncopyable {
    public:
      static PythonBoxPointerPool& get_instance();
      std::shared_ptr<pybind11::object> get(const pybind11::object& obj);
      void collect_garbage();

    private:
      PythonBoxPointerPool() = default;

      std::vector<std::weak_ptr<pybind11::object>> m_pool;
      std::mutex m_mutex;
  };

  template<typename T>
  class PythonBox : private Noncopyable {
    public:
      using Type = T;

      PythonBox(const pybind11::object& obj)
        : m_obj(PythonBoxPointerPool::get_instance().get(obj)) {}

      PythonBox(PythonBox&&) = default;

      Type generate(Evaluator& e) {
        auto evaluator_ptr = std::shared_ptr<Evaluator>(&e, [](auto) {});
        auto evaluator_handle = pybind11::cast(evaluator_ptr).release();
        auto value = m_obj->attr("generate")(evaluator_handle).cast<Type>();
        return value;
      }

      pybind11::object* operator &() {
        return m_obj.get();
      }

      const pybind11::object* operator &() const {
        return m_obj.get();
      }

      ~PythonBox() {
        m_obj.reset();
        PythonBoxPointerPool::get_instance().collect_garbage();
      }

    private:
      std::shared_ptr<pybind11::object> m_obj;
  };

  bool is_python_generator(const pybind11::object& arg);
}

namespace Rover {
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
