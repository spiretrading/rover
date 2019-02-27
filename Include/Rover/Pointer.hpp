#ifndef ROVER_POINTER_HPP
#define ROVER_POINTER_HPP
#include <type_traits>

namespace Rover {

  /** Trait to check whether a type is a pointer to a C++ object or POD
      type. */
  template<typename, typename = void>
  struct is_object_pointer : std::false_type {};

  template<typename T>
  struct is_object_pointer<T, std::enable_if_t<std::is_pointer_v<T>>> :
    std::true_type {};

  template<typename T>
  struct is_object_pointer<T, std::enable_if_t<decltype(*std::declval<T>(),
    std::declval<T>().operator->(), std::true_type{})::value>> :
    std::true_type {};

  /** Trait to check whether a type is a pointer to a C++ object or POD
      type. */
  template<typename T>
  inline constexpr bool is_object_pointer_v = is_object_pointer<T>::value;
 
  /** Trait to retrieve the type of values produced by a generator even if a
      pointer is passed. */
  template<typename Generator>
  using generator_type = std::conditional_t<is_object_pointer_v<
    Generator>, typename std::pointer_traits<Generator>::element_type,
    typename Generator>;

  /** Trait to retrieve the type of values produced by a generator even if a
      pointer is passed. */
  template<typename Generator>
  using generator_type_v = typename generator_type<Generator>::Type;
}

#endif
