#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

namespace Rover {

  template<typename Generator>
  class Reference;

  template<typename Type>
  struct is_reference : std::false_type {};

  template<typename Generator>
  struct is_reference<Reference<Generator>> : std::true_type {};

  template<typename Type>
  inline constexpr bool is_reference_v = is_reference<Type>::value;

  namespace Session {

    template<typename Reference>
    struct Entry;

    template<typename Type>
    struct is_entry : std::false_type {};

    template<typename Reference>
    struct is_entry<Entry<Reference>> : std::true_type {};

    template<typename Type>
    inline constexpr bool is_entry_v = is_entry<Type>::value;

    template<typename Type>
    struct return_value;

    template<typename Reference>
    struct return_value<Entry<Reference>> {
      using type = typename Reference::Type;
    };

    template<typename Type>
    using return_value_t = typename return_value<Type>::type;
  }

}

#endif