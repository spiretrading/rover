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
}

namespace pybind11 {
  object abs(const object& obj);
  object floor(const object& obj);

  template<typename T1, typename T2, std::enable_if_t<
    Rover::Details::contain_objects_v<T1, T2>>* = nullptr>
  bool operator <(const T1& lhs, const T2& rhs) {
    return Rover::Details::import_operator().attr("__lt__")(lhs,
      rhs).cast<bool>();
  }

  template<typename T1, typename T2, std::enable_if_t<
    Rover::Details::contain_objects_v<T1, T2>>* = nullptr>
  bool operator <=(const T1& lhs, const T2& rhs) {
    return Rover::Details::import_operator().attr("__le__")(lhs,
      rhs).cast<bool>();
  }

  template<typename T1, typename T2, std::enable_if_t<
    Rover::Details::contain_objects_v<T1, T2>>* = nullptr>
  bool operator >(const T1& lhs, const T2& rhs) {
    return Rover::Details::import_operator().attr("__gt__")(lhs,
      rhs).cast<bool>();
  }

  template<typename T1, typename T2, std::enable_if_t<
    Rover::Details::contain_objects_v<T1, T2>>* = nullptr>
  bool operator >=(const T1& lhs, const T2& rhs) {
    return Rover::Details::import_operator().attr("__ge__")(lhs,
      rhs).cast<bool>();
  }

  template<typename T1, typename T2, std::enable_if_t<
    Rover::Details::contain_objects_v<T1, T2>>* = nullptr>
  bool operator ==(const T1& lhs, const T2& rhs) {
    return Rover::Details::import_operator().attr("__eq__")(lhs,
      rhs).cast<bool>();
  }

  template<typename T1, typename T2, std::enable_if_t<
    Rover::Details::contain_objects_v<T1, T2>>* = nullptr>
  bool operator !=(const T1& lhs, const T2& rhs) {
    return Rover::Details::import_operator().attr("__ne__")(lhs,
      rhs).cast<bool>();
  }
  
  template<typename T1, typename T2, std::enable_if_t<
    Rover::Details::contain_objects_v<T1, T2>>* = nullptr>
  object operator +(const T1& lhs, const T2& rhs) {
    return Rover::Details::import_operator().attr("__add__")(lhs,
      rhs);
  }
  
  template<typename T1, typename T2, std::enable_if_t<
    Rover::Details::contain_objects_v<T1, T2>>* = nullptr>
  object operator -(const T1& lhs, const T2& rhs) {
    return Rover::Details::import_operator().attr("__sub__")(lhs,
      rhs);
  }

  template<typename T1, typename T2, std::enable_if_t<
    Rover::Details::contain_objects_v<T1, T2>>* = nullptr>
  object operator *(const T1& lhs, const T2& rhs) {
    return Rover::Details::import_operator().attr("__mul__")(lhs,
      rhs);
  }

  template<typename T1, typename T2, std::enable_if_t<
    Rover::Details::contain_objects_v<T1, T2>>* = nullptr>
  object operator /(const T1& lhs, const T2& rhs) {
    return Rover::Details::import_operator().attr("__floordiv__")(lhs,
      rhs);
  }

  template<typename T1, typename T2, std::enable_if_t<
    Rover::Details::contain_objects_v<T1, T2>>* = nullptr>
  object operator %(const T1& lhs, const T2& rhs) {
    return Rover::Details::import_operator().attr("__mod__")(lhs,
      rhs);
  }
}

#endif
