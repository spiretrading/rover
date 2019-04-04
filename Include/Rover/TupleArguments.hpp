#ifndef ROVER_TUPLE_ARGUMENTS_HPP
#define ROVER_TUPLE_ARGUMENTS_HPP
#include <tuple>

namespace Rover {

  //! Applies a function to every element of a tuple
  /*!
    \param func Function taking a tuple element and its index as arguments.
    \param tuple Tuple of the arguments.
  */
  template<typename Func, typename... A>
  void visit_arguments(Func&& func, std::tuple<A...>& tuple) {
    std::apply([&](auto&&... args) {
      auto i = 0LU;
      (func(args, i++), ...);
    }, tuple);
  }

  //! Returns the number of elements in a tuple
  template<typename... A>
  constexpr std::size_t arguments_size(const std::tuple<A...>& tuple) {
    return std::tuple_size_v<std::tuple<A...>>;
  }
}

#endif
