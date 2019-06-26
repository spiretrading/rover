#include <algorithm>
#include "Autobox.hpp"

using namespace pybind11;

namespace Rover::Details {
  PythonBoxPointerPool& PythonBoxPointerPool::get_instance() {
    static auto pool = PythonBoxPointerPool();
    return pool;
  }

  std::shared_ptr<object> PythonBoxPointerPool::get(const object& obj) {
    auto lk = std::lock_guard(m_mutex);
    auto it = std::find_if(m_pool.begin(), m_pool.end(), [&](auto& o) {
      return o.lock()->is(obj);
    });
    if(it == m_pool.end()) {
      auto ptr = std::make_shared<object>(obj);
      m_pool.push_back(ptr);
      return ptr;
    }
    return it->lock();
  }

  void PythonBoxPointerPool::collect_garbage() {
    auto lk = std::lock_guard(m_mutex);
    while(true) {
      auto it = std::find_if(m_pool.begin(), m_pool.end(), [&](auto& o) {
        return o.expired();
      });
      if(it == m_pool.end()) {
        break;
      } else {
        m_pool.erase(it);
      }
    }
  }

  bool is_python_generator(const pybind11::object& arg) {
    return pybind11::hasattr(arg, "generate") &&
      PyCallable_Check(arg.attr("generate").ptr());
  }
}
