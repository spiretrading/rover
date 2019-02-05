#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <limits>
#include <variant>

namespace Rover {

  namespace Details {

    template<typename Tuple, typename Type, size_t I>
    struct contains {
      const static bool value = std::is_same_v<std::tuple_element_t<I, Tuple>,
        Type> || contains<Tuple, Type, I - 1>::value;
    };

    template<typename Tuple, typename Type>
    struct contains<Tuple, Type, 0> {
      const static bool value = std::is_same_v<std::tuple_element_t<0, Tuple>,
        Type>;
    };
    
    template<typename Tuple, size_t I, size_t J = std::tuple_size_v<Tuple> - 1>
    struct remove {
      using type = decltype(std::tuple_cat(
        std::declval<typename remove<Tuple, I, J - 1>::type>(),
        std::make_tuple(std::declval<std::tuple_element_t<J,
        Tuple>>())));
    };

    template<typename Tuple, size_t I>
    struct remove<Tuple, I, I> {
      using type = typename remove<Tuple, I, I - 1>::type;
    };

    template<typename Tuple, size_t I>
    struct remove<Tuple, I, std::numeric_limits<size_t>::max()> {
      using type = std::tuple<>;
    };
  }

  template<typename...> struct typelist;
  
  template<typename Tuple, size_t I = std::tuple_size_v<Tuple> - 1>
  struct remove_duplicates {
    using type = std::conditional_t<
      Details::contains<
        Tuple,
        std::tuple_element_t<I, Tuple>,
        I - 1
      >::value,
      typename remove_duplicates<
        typename Details::remove<Tuple, I>::type,
        I - 1
      >::type,
      typename remove_duplicates<Tuple, I - 1>::type
    >;
  };

  template<size_t I>
  struct remove_duplicates<std::tuple<>, I> {
    using type = std::tuple<>;
  };

  template<typename Tuple>
  struct remove_duplicates<Tuple, 0> {
    using type = Tuple;
  };

  template<typename Tuple>
  using remove_duplicates_t = typename remove_duplicates<Tuple>::type;

  template<typename Tuple>
  struct to_variant;

  template<typename... Args>
  struct to_variant<std::tuple<Args...>> {
    using type = std::variant<Args...>;
  };

  template<typename Tuple>
  using to_variant_t = typename to_variant<Tuple>::type;

}

#endif