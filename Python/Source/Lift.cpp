#include "Rover/Python/Lift.hpp"
#include <vector>
#include <pybind11/functional.h>
#include "Rover/Python/Autobox.hpp"
#include "Rover/Python/Box.hpp"
#include "Rover/Python/Evaluator.hpp"

using namespace pybind11;
using namespace Rover;

namespace {
  using ArgsCont = std::vector<Box<object>>;
  using KwargsCont = std::vector<std::pair<handle, Box<object>>>;

  template<typename Func>
  ArgsCont args_apply(Func&& func, tuple&& args) {
    auto result = ArgsCont();
    for(auto i = 0U; i < args.size(); ++i) {
      result.push_back(func(std::move(args[i])));
    }
    return result;
  }

  template<typename Func>
  KwargsCont args_apply(Func&& func, dict&& args) {
    auto result = KwargsCont();
    for(auto& arg : args) {
      auto value = func(reinterpret_borrow<object>(std::move(arg.second)));
      result.emplace_back(std::move(arg.first), std::move(value));
    }
    return result;
  }

  template<typename Func>
  tuple args_apply(Func&& func, ArgsCont& cont) {
    auto result = tuple(cont.size());
    for(auto i = 0U; i < cont.size(); ++i) {
      result[i] = func(cont[i]);
    }
    return result;
  }

  template<typename Func>
  dict args_apply(Func&& func, KwargsCont& cont) {
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
          m_args(args_apply(python_autobox<object, object>,
            std::move(args))),
          m_kwargs(args_apply(python_autobox<object, object>,
            std::move(kwargs))) {}

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

void Rover::export_lift(module& module) {
  class_<PythonLift>(module, "Lift")
    .def(init<object, args, kwargs>())
    .def("generate", &PythonLift::generate);
  implicitly_convertible<PythonLift, Box<object>>();
}
