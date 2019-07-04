#ifndef ROVER_PYTHON_BASIS_HPP
#define ROVER_PYTHON_BASIS_HPP
#include <type_traits>
#include <variant>
#include <vector>
#include <pybind11/pybind11.h>
#include "Rover/Basis.hpp"
#include "Rover/Factor.hpp"
#include "Sample.hpp"

namespace Rover {

  //! Specializes BasisArguments for PythonSample arguments.
  template<>
  class BasisArguments<PythonSample::Arguments> {
    public:
      explicit BasisArguments(const PythonSample::Arguments& args)
          : m_arguments(args.size()) {
        for(auto i = std::size_t(0); i < args.size(); ++i) {
          m_arguments[i] = args[i];
        }
      }

      template<typename T>
      void set_value(std::size_t index, T&& value) {
        m_arguments[index] = std::forward<T>(value);
      }

      template<typename T>
      void add_category(std::size_t index, T&& category) {
        auto visited = false;
        std::visit([&](auto& factor) {
          if constexpr(std::is_same_v<std::decay_t<decltype(factor)>,
             Factor<pybind11::object>>) {
            factor.add_category(std::forward<T>(category));
            visited = true;
          }
        }, m_arguments[index]);
        if(!visited) {
          auto factor = Factor<pybind11::object>();
          factor.add_category(std::forward<T>(category));
          m_arguments[index] = std::move(factor);
        }
      }

      std::size_t size() const {
        return m_arguments.size();
      }

      template<typename Func>
      void visit(Func&& func) {
        for(auto i = std::size_t(0); i < m_arguments.size(); ++i) {
          func(m_arguments[i], i);
        }
      }

      template<typename Func>
      void visit(Func&& func) const {
        for(auto i = std::size_t(0); i < m_arguments.size(); ++i) {
          func(m_arguments[i], i);
        }
      }
      
    private:
      std::vector<std::variant<pybind11::object, Factor<pybind11::object>>>
        m_arguments;
  };
}

#endif
