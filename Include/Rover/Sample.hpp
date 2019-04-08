#ifndef ROVER_SAMPLE_HPP
#define ROVER_SAMPLE_HPP
#include <tuple>

namespace Rover {

  //! Represents a single execution of a function of a set of parameters.
  /*!
    \tparam R The type of the result.
    \tparam A The types of the arguments.
  */
  template<typename R, typename... A>
  struct Sample {

    //! The type of the result.
    using Result = R;

    //! The type of the arguments.
    using Arguments = std::tuple<A...>;

    //! The result of the function.
    Result m_result;

    //! The arguments passed to the function.
    Arguments m_arguments;
  };

  //! Applies a function to every element of a tuple.
  /*!
    \param func Function taking a tuple element and its index as arguments.
    \param tuple Tuple of the arguments.
  */
  template<typename Func, typename... A>
  void visit_arguments(Func&& func, std::tuple<A...>& tuple) {
    std::apply([&](auto&&... args) {
      auto i = std::size_t(0);
      (func(args, i++), ...);
    }, tuple);
  }

  //! Applies a function to every element of a tuple.
  /*!
    \param func Function taking a tuple element and its index as arguments.
    \param tuple Tuple of the arguments.
  */
  template<typename Func, typename... A>
  void visit_arguments(Func&& func, const std::tuple<A...>& tuple) {
    std::apply([&](auto&&... args) {
      auto i = std::size_t(0);
      (func(args, i++), ...);
    }, tuple);
  }

  //! Returns the number of elements in a tuple.
  template<typename... A>
  constexpr std::size_t arguments_size(const std::tuple<A...>& tuple) {
    return std::tuple_size_v<std::tuple<A...>>;
  }
}

#endif
