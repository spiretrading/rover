#ifndef ROVER_PYTHON_MODEL_HPP
#define ROVER_PYTHON_MODEL_HPP
#include <string_view>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "Rover/Model.hpp"
#include "Arithmetics.hpp"
#include "Basis.hpp"
#include "Scalar.hpp"
#include "VariableTraits.hpp"

namespace Rover {
namespace Details {
  template<typename T>
  class PythonAlgorithm {
    public:
      using Type = T;

      using ClientType = typename Type::Type;

      using ScalarViewType = ScalarView<std::function<ScalarSample<
        ClientType> (std::size_t)>>;

      explicit PythonAlgorithm(pybind11::object impl)
        : m_impl(std::move(impl)) {}

      template<typename ScalarView>
      void learn(const ScalarView& view) {
        auto raw_getter = [view](std::size_t index) {
          return sample_cast(view[index]);
        };
        auto raw_view = ScalarViewType(raw_getter, view.size());
        m_impl.attr("learn")(raw_view);
      }

      template<typename Arguments>
      Type predict(const Arguments& args) const {
        auto value = m_impl.attr("predict")(arguments_cast(
          args)).template cast<ClientType>();
        auto result = static_cast<Type>(value);
        return result;
      }

    private:
      pybind11::object m_impl;

      static typename ScalarSample<ClientType>::Arguments arguments_cast(const
          typename ScalarSample<Type>::Arguments& arguments) {
        auto result = typename ScalarSample<ClientType>::Arguments();
        std::copy(arguments.begin(), arguments.end(), std::back_inserter(
          result));
        return result;
      }

      static ScalarSample<ClientType> sample_cast(const ScalarSample<Type>&
          sample) {
        auto result = ScalarSample<ClientType>();
        result.m_result = static_cast<ClientType>(sample.m_result);
        result.m_arguments = arguments_cast(sample.m_arguments);
        return result;
      }
  };
}

  //! Exports an instantiation of the Model class. 
  /*
    \tparam T The type of the Trial.
    \tparam S The raw arithmetic type of the scalar.
    \param module The module to export the class to.
    \param suffix The suffix to the name of the exported class.
  */
  template<typename T, typename S>
  void export_model(pybind11::module& module, std::string_view suffix);

  //! Exports Model<TrialView<PythonSample>, double> as the default Model.
  void export_model(pybind11::module& module);

  template<typename T, typename S>
  void export_model(pybind11::module& module, std::string_view suffix) {
    using ScalarType = PythonScalar<S>;
    using AlgorithmType = Details::PythonAlgorithm<ScalarType>;
    using TrialType = T;
    using ModelType = Model<AlgorithmType, TrialType>;
    auto name = std::string("Model").append(suffix);
    pybind11::class_<ModelType>(module, name.c_str())
      .def(pybind11::init([](const TrialType& t, pybind11::object impl) {
         return ModelType(t, AlgorithmType(std::move(impl)));
       }))
      .def("__call__", &ModelType::operator ());
  }
}

#endif
