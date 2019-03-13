#include <vector>
#include "pybind11/functional.h"
#include "Autobox.hpp"
#include "Rover/Box.hpp"
#include "Rover/Evaluator.hpp"

using namespace pybind11;

namespace {
  using namespace Rover;
  using ArgsCont = std::vector<Box<object>>;
  using KwargsCont = std::vector<std::pair<handle,
    Box<object>>>;
  
  ArgsCont args_apply(const std::function<Box<object>(object&&)>& func,
      tuple&& args) {
    auto result = ArgsCont();
    for(auto i = 0U; i < args.size(); ++i) {
      result.push_back(func(std::move(args[i])));
    }
    return result;
  }

  KwargsCont args_apply(const std::function<Box<object>(object&&)>& func,
      dict&& args) {
    auto result = KwargsCont();
    for(auto& arg : args) {
      auto value = func(reinterpret_borrow<object>(std::move(arg.second)));
      result.emplace_back(std::move(arg.first), std::move(value));
    }
    return result;
  }

  tuple args_apply(const std::function<object(Box<object>&)>& func,
      ArgsCont& cont) {
    auto result = tuple(cont.size());
    for(auto i = 0U; i < cont.size(); ++i) {
      result[i] = func(cont[i]);
    }
    return result;
  }

  dict args_apply(const std::function<object(Box<object>&)>& func,
      KwargsCont& cont) {
    auto result = dict();
    for(auto& arg : cont) {
      result[arg.first] = func(arg.second);
    }
    return result;
  }

  class PythonLift {
    public:
      using Type = object;

      PythonLift(object callable, args args, kwargs kwargs)
        : m_callable(std::move(callable)),
          m_args(args_apply([](object&& generator) {
            return python_autobox<object>(std::move(generator));
          }, std::move(args))),
          m_kwargs(args_apply([](object&& generator) {
            return python_autobox<object>(std::move(generator));
          }, std::move(kwargs))) {}

      Type generate(Evaluator& evaluator) {
        auto args = args_apply([&](Box<object>& generator) {
          return evaluator.evaluate(generator);
        }, m_args);
        auto kwargs = args_apply([&](Box<object>& generator) {
          return evaluator.evaluate(generator);
        }, m_kwargs);
        return m_callable(*args, **kwargs);
      }

    private:
      object m_callable;
      ArgsCont m_args;
      KwargsCont m_kwargs;
  };
}

namespace Rover {
  void export_lift(pybind11::module& module) {
    class_<PythonLift>(module, "Lift")
      .def(init<object, args, kwargs>())
      .def("generate", &PythonLift::generate);

    implicitly_convertible<PythonLift, Box<object>>();
  }
}
