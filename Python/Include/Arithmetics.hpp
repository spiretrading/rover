#ifndef ROVER_PYTHON_ARITHMETICS_HPP
#define ROVER_PYTHON_ARITHMETICS_HPP
#include <type_traits>
#include <pybind11/pybind11.h>

namespace Rover::Details {
  template<typename... Args>
  struct contain_objects {
    static constexpr bool value = std::disjunction_v<std::is_same<Args,
      pybind11::object>...>;
  };

  template<typename... Args>
  inline constexpr bool contain_objects_v = contain_objects<Args...>::value;

  const pybind11::module& import_operator();

  template<typename T1, typename T2>
  struct OperatorApplicator;

  template<typename T>
  struct OperatorApplicator<T, pybind11::object> {
    pybind11::object operator()(const char* operator_name, const T& lhs,
        const pybind11::object& rhs) {
      auto lhs_object = pybind11::cast(lhs);
      return import_operator().attr(operator_name)(lhs_object, rhs);
    }
  };

  template<typename T>
  struct OperatorApplicator<pybind11::object, T> {
    pybind11::object operator()(const char* operator_name, const
        pybind11::object& lhs, const T& rhs) {
      auto rhs_object = pybind11::cast(rhs);
      return import_operator().attr(operator_name)(lhs, rhs_object);
    }
  };

  template<>
  struct OperatorApplicator<pybind11::object, pybind11::object> {
    pybind11::object operator()(const char* operator_name, const
        pybind11::object& lhs, const pybind11::object& rhs) {
      return import_operator().attr(operator_name)(lhs, rhs);
    }
  };
}

namespace pybind11 {
  object abs(const object& obj);
  object floor(const object& obj);

  template<typename T1, typename T2>
  std::enable_if_t<Rover::Details::contain_objects_v<T1, T2>, bool> 
      operator <(const T1& lhs, const T2& rhs) {
    return Rover::Details::OperatorApplicator<T1, T2>()("__lt__", lhs,
      rhs).template cast<bool>();
  }

  template<typename T1, typename T2>
  std::enable_if_t<Rover::Details::contain_objects_v<T1, T2>, bool>
      operator <=(const T1& lhs, const T2& rhs) {
    return Rover::Details::OperatorApplicator<T1, T2>()("__le__", lhs,
      rhs).template cast<bool>();
  }

  template<typename T1, typename T2>
  std::enable_if_t<Rover::Details::contain_objects_v<T1, T2>, bool>
      operator >(const T1& lhs, const T2& rhs) {
    return Rover::Details::OperatorApplicator<T1, T2>()("__gt__", lhs,
      rhs).template cast<bool>();
  }

  template<typename T1, typename T2>
  std::enable_if_t<Rover::Details::contain_objects_v<T1, T2>, bool>
      operator >=(const T1& lhs, const T2& rhs) {
    return Rover::Details::OperatorApplicator<T1, T2>()("__ge__", lhs,
      rhs).template cast<bool>();
  }

  template<typename T1, typename T2>
  std::enable_if_t<Rover::Details::contain_objects_v<T1, T2>, bool>
      operator ==(const T1& lhs, const T2& rhs) {
    return Rover::Details::OperatorApplicator<T1, T2>()("__eq__", lhs,
      rhs).template cast<bool>();
  }

  template<typename T1, typename T2>
  std::enable_if_t<Rover::Details::contain_objects_v<T1, T2>, bool>
      operator !=(const T1& lhs, const T2& rhs) {
    return Rover::Details::OperatorApplicator<T1, T2>()("__ne__", lhs,
      rhs).template cast<bool>();
  }
  
  template<typename T1, typename T2>
  std::enable_if_t<Rover::Details::contain_objects_v<T1, T2>, object>
      operator +(const T1& lhs, const T2& rhs) {
    return Rover::Details::OperatorApplicator<T1, T2>()("__add__", lhs, rhs);
  }
  
  template<typename T1, typename T2>
  std::enable_if_t<Rover::Details::contain_objects_v<T1, T2>, object>
      operator -(const T1& lhs, const T2& rhs) {
    return Rover::Details::OperatorApplicator<T1, T2>()("__sub__", lhs, rhs);
  }

  template<typename T1, typename T2>
  std::enable_if_t<Rover::Details::contain_objects_v<T1, T2>, object>
      operator *(const T1& lhs, const T2& rhs) {
    return Rover::Details::OperatorApplicator<T1, T2>()("__mul__", lhs, rhs);
  }

  template<typename T1, typename T2>
  std::enable_if_t<Rover::Details::contain_objects_v<T1, T2>, object>
      operator /(const T1& lhs, const T2& rhs) {
    return Rover::Details::OperatorApplicator<T1, T2>()("__floordiv__", lhs,
      rhs);
  }

  template<typename T1, typename T2>
  std::enable_if_t<Rover::Details::contain_objects_v<T1, T2>, object>
      operator %(const T1& lhs, const T2& rhs) {
    return Rover::Details::OperatorApplicator<T1, T2>()("__mod__", lhs, rhs);
  }
}

#endif
