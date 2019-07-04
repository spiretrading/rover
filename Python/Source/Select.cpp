#include "Rover/Python/Select.hpp"
#include "Rover/Python/Arithmetics.hpp"
#include "Rover/Python/Autobox.hpp"
#include "Rover/Python/Range.hpp"

using namespace Rover;
using namespace pybind11;

namespace {
  class PythonSelect {
    public:
      using Container = object;
      using Selector = object;
      using Type = object;

      explicit PythonSelect(Container container)
        : m_container(convert_container(container, false)),
          m_selector(create_default_selector(m_container.attr("__len__")())) {}

      PythonSelect(Container container, Selector selector)
        : m_container(convert_container(container, true)),
          m_selector(python_autobox<object>(std::move(selector))) {}

      Type generate(Evaluator& evaluator) {
        auto index = evaluator.evaluate(m_selector);
        auto element = m_container.attr("__getitem__")(index);
        auto generator = python_autobox<object>(element);
        auto value = evaluator.evaluate(generator);
        return value;
      }

    private:
      static Box<object> create_default_selector(const object& size) {
        auto begin = cast(0);
        auto end = size - 1;
        auto range = Details::PythonRange(std::move(begin), std::move(end),
          cast(1));
        auto selector = python_autobox<object>(cast(range,
          return_value_policy::move));
        return selector;
      }

      static Container convert_container(Container& container,
          bool keep_dict) {
        if(is_dict(container)) {
          if(keep_dict) {
            return std::move(container);
          } else {
            auto view = container.attr("values")();
            return list(view);
          }
        } else if(is_list(container)) {
          return std::move(container);
        } else if(is_set(container)) {
          return list(container); 
        } else {
          throw std::runtime_error(
            "Container does not implement a list, set, or dict.");
        }
      }

      static bool is_set(const object& obj) {
        return !hasattr(obj, "__getitem__");
      }

      static bool is_list(const object& obj) {
        try {
          obj.attr("__getitem__")(cast(0));
        } catch(const pybind11::index_error&) {
          return true;
        } catch(const pybind11::error_already_set&) {
          return false;
        }
        return true;
      }

      static bool is_dict(const object& obj) {
        return hasattr(obj, "values");
      }

    private:
      Container m_container;
      Box<object> m_selector;
  };
}

void Rover::export_select(module& module) {
  class_<PythonSelect>(module, "Select")
    .def(init<object>())
    .def(init<object, object>())
    .def("generate", &PythonSelect::generate);
  implicitly_convertible<PythonSelect, Box<object>>();
}
