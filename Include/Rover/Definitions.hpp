#ifndef ROVER_DEFINITIONS_HPP
#define ROVER_DEFINITIONS_HPP

namespace Rover {

  template<typename Generator>
  class Reference;

  template<typename Type>
  struct is_reference : std::false_type {};

  template<typename Generator>
  struct is_reference<Reference<Generator>> : std::true_type {};

  template<typename Type>
  inline constexpr bool is_reference_v = is_reference<Type>::value;

  template<typename Reference>
  struct SessionEntry;

  template<typename Type>
  struct is_session_entry : std::false_type {};

  template<typename Reference>
  struct is_session_entry<SessionEntry<Reference>> : std::true_type {};

  template<typename Type>
  inline constexpr bool is_session_entry_v = is_session_entry<Type>::value;

  template<typename Type>
  struct session_entry_return_value;

  template<typename Reference>
  struct session_entry_return_value<SessionEntry<Reference>> {
    using type = typename Reference::Type;
  };

  template<typename Type>
  using session_entry_return_value_t =
    typename session_entry_return_value<Type>::type;
}

#endif
