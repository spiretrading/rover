#ifndef ROVER_PYTHON_SAMPLE_HPP
#define ROVER_PYTHON_SAMPLE_HPP
#include <functional>
#include <optional>
#include <string_view>
#include <tuple>
#include <utility>
#include <pybind11/pybind11.h>
#include "Rover/Noncopyable.hpp"

namespace Rover {

  //! Sample containing Python objects.
  struct PythonSample {
    using Result = pybind11::object;
    using Arguments = pybind11::tuple;

    //! The result of the function.
    Result m_result;

    //! The arguments passed to the function.
    Arguments m_arguments;
  };

  //! Applies a function to every element of a Python tuple.
  /*!
    \param func Function taking a tuple element and its index as arguments.
    \param tuple Python tuple of the arguments.
  */
  template<typename Func>
  void visit_arguments(Func&& func, pybind11::tuple& tuple) {
    for(auto i = std::size_t(0); i < tuple.size(); ++i) {
      func(tuple[i], i);
    }
  }

  //! Applies a function to every element of a Python tuple.
  /*!
    \param func Function taking a tuple element and its index as arguments.
    \param tuple Python tuple of the arguments.
  */
  template<typename Func>
  void visit_arguments(Func&& func, const pybind11::tuple& tuple) {
    for(auto i = std::size_t(0); i < tuple.size(); ++i) {
      func(tuple[i], i);
    }
  }

  //! Returns the number of elements in a Python tuple.
  std::size_t arguments_size(const pybind11::tuple& tuple);
}

namespace Rover {
namespace Details {
  template<typename T, std::size_t... I>
  T cast_arguments(const pybind11::tuple& t, std::index_sequence<I...>) {
    return std::make_tuple(t[I].template cast<std::tuple_element_t<I, T>>()...);
  }

  template<typename S>
  S sample_cast(const PythonSample& s) {
    auto result = S{ s.m_result.template cast<typename S::Result>(),
      cast_arguments<typename S::Arguments>(s.m_arguments,
      std::make_index_sequence<std::tuple_size_v<typename S::Arguments>>()) };
    return result;
  }

  class SampleConverter : private Noncopyable {
    public:
      static SampleConverter& get_instance() {
        static auto converter = SampleConverter();
        return converter;
      }

      PythonSample cast(const pybind11::object& sample) const {
        auto result = m_caster(sample);
        return *result;
      }

      template<typename S>
      void register_sample() {
        m_caster = [caster = std::move(m_caster)](
            const pybind11::object& object) -> std::optional<PythonSample> {
          auto value = caster(object);
          if(value) {
            return value;
          }
          try {
            auto sample = object.template cast<S>();
            auto result = PythonSample{
              pybind11::cast(sample.m_result),
              std::apply([](auto&&... args) {
                return pybind11::make_tuple(std::move(args)...);
              }, std::move(sample.m_arguments))
            };
            return result;
          } catch(const pybind11::cast_error&) {
            return std::nullopt;
          }
        };
      }

    private:
      std::function<std::optional<PythonSample>(const pybind11::object&)> m_caster;

      SampleConverter()
        : m_caster([](const pybind11::object&) -> std::optional<PythonSample> {
            return std::nullopt;
          }) {}
  };
}

  //! Exports a Sample for Python objects.
  /*!
    \param module The module to export the class to.
  */
  void export_sample(pybind11::module& module);

  //! Exports a Sample for specified types.
  /*!
    \tparam R The type of the result.
    \tparam P The types of the parameters.
    \param module The module to export the class to.
    \param suffix The suffix to the name of the exported class.
  */
  template<typename R, typename... P>
  void export_sample(pybind11::module& module, std::string_view suffix);

  template<typename R, typename... P>
  void export_sample(pybind11::module& module, std::string_view suffix) {
    auto name = std::string("Sample").append(suffix);
    pybind11::class_<Sample<R, P...>>(module, name.c_str())
      .def(pybind11::init<>())
      .def(pybind11::init<typename Sample<R, P...>::Result,
         typename Sample<R, P...>::Arguments>())
      .def(pybind11::init(
         [](const PythonSample& s) {
           return Details::sample_cast<Sample<R, P...>>(s);
         }))
      .def_readwrite("result", &Sample<R, P...>::m_result)
      .def_readwrite("arguments", &Sample<R, P...>::m_arguments);
    Details::SampleConverter::get_instance().register_sample<Sample<R, P...>>();
    pybind11::implicitly_convertible<Sample<R, P...>, PythonSample>();
    pybind11::implicitly_convertible<PythonSample, Sample<R, P...>>();
  }
}

#endif
