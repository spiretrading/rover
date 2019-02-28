#ifndef ROVER_POINTER_HPP
#define ROVER_POINTER_HPP
#include <type_traits>
#include "Generator.hpp"

namespace Rover {

  /** Trait to check whether a type is a pointer to a C++ object or POD
      type. */
  template<typename, typename = void>
  struct is_object_pointer;

  /** Trait to check whether a type is a pointer to a C++ object or POD
    type. */
  template<typename T>
  inline constexpr bool is_object_pointer_v = is_object_pointer<T>::value;

  /** Trait to return a pointer-to type if tested type is a pointer */
  template<typename>
  struct dereference_if_pointer;

  /** Trait to return a pointer-to type if tested type is a pointer */
  template<typename T>
  using dereference_if_pointer_t = typename dereference_if_pointer<T>::type;

  /** Trait to check whether a type represent a (pointer to) Generator */
  template<typename, typename>
  struct is_generator_handle;

  /** Trait to check whether a type represent a (pointer to) Generator */
  template<typename T>
  inline constexpr bool is_generator_handle_v = is_generator_handle<T>::value;

  /** Trait to check whether a type represent a pointer to a Generator */
  template<typename, typename>
  struct is_generator_pointer;

  /** Trait to check whether a type represent a pointer to a Generator */
  template<typename>
  inline constexpr bool is_generator_pointer_v =
    is_generator_pointer<T>::value;

  /** Trait to retrieve the type of values produced by a generator even if a
      pointer is passed. */
  template<typename Generator>
  struct generator_type {
    using type = typename dereference_if_pointer_t<Generator>::Type;
  };

  /** Trait to retrieve the type of values produced by a generator even if a
      pointer is passed. */
  template<typename Generator>
  using generator_type_t = typename generator_type<Generator>::type;

  template<typename, typename>
  struct is_object_pointer : std::false_type {};

  template<typename T>
  struct is_object_pointer<T, std::enable_if_t<std::is_pointer_v<T>>> :
    std::true_type {
  };

  template<typename T>
  struct is_object_pointer<T, std::enable_if_t<decltype(*std::declval<T>(),
    std::declval<T>().operator->(), std::true_type{})::value>> :
    std::true_type {};

  template<typename T>
  struct dereference_if_pointer {
    using type = std::conditional_t<is_object_pointer_v<T>,
      typename std::pointer_traits<T>::element_type, T>;
  };

  template<typename, typename>
  struct is_generator_handle : std::false_type {};

  template<typename T>
  struct is_generator_handle<T, std::enable_if_t<is_generator_v<
    dereference_if_pointer_t<T>>>> : std::true_type {};

  template<typename, typename>
  struct is_generator_pointer : std::false_type {};

  template<typename T>
  struct is_generator_pointer<T, std::enable_if_t<is_generator_handle_v<T> &&
    is_object_pointer_v<T>>> : std::true_type {};
}

#endif
