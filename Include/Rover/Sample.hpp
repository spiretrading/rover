#ifndef ROVER_SAMPLE_HPP
#define ROVER_SAMPLE_HPP
#include <iostream>
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
    \param func Function taking an argument and its index as arguments.
    \param args Tuple of the arguments.
  */
  template<typename Arguments, typename Func>
  void visit_arguments(Func&& func, Arguments& args);

  //! Applies a function to every argument.
  /*!
    \param func Function taking an argument and its index as arguments.
    \param args Tuple of the arguments.
  */
  template<typename Arguments, typename Func>
  void visit_arguments(Func&& func, const Arguments& args);

  //! Returns the number of the arguments.
  template<typename Arguments>
  constexpr std::size_t arguments_size(const Arguments& args);

  //! Serializes a Sample in the comma-separated format.
  template<typename R, typename... A>
  std::ostream& operator <<(std::ostream& stream, const Sample<R, A...>&
    sample);

  //! De-serialized a Sample from the comma-separated format.
  template<typename R, typename... A>
  std::istream& operator >>(std::istream& stream, Sample<R, A...>& sample);

  //! Exposes visitor and size functionality for generic Sample arguments.
  /*
    \tparam A The type of the arguments.
  */
  template<typename A>
  struct ArgumentVisitor {

    //! The type of the arguments. 
    using Arguments = A;

    //! Applies a function to every element of a tuple.
    /*!
      \param func Function taking a tuple element and its index as arguments.
      \param args Tuple of the arguments.
    */
    template<typename Func>
    static void visit(Func&& func, Arguments& tuple);

    //! Applies a function to every element of a tuple.
    /*!
      \param func Function taking a tuple element and its index as arguments.
      \param tuple Tuple of the arguments.
    */
    template<typename Func>
    static void visit(Func&& func, const Arguments& tuple);

    //! Returns the number of elements in a tuple.
    static constexpr std::size_t size(const Arguments& tuple);
  };

  template<typename Arguments, typename Func>
  void visit_arguments(Func&& func, Arguments& args) {
    return ArgumentVisitor<Arguments>::visit(std::forward<Func>(func), args);
  }

  template<typename Arguments, typename Func>
  void visit_arguments(Func&& func, const Arguments& args) {
    return ArgumentVisitor<Arguments>::visit(std::forward<Func>(func), args);
  }

  template<typename Arguments>
  constexpr std::size_t arguments_size(const Arguments& args) {
    return ArgumentVisitor<Arguments>::size(args);
  }
  
  template<typename R, typename... A>
  std::ostream& operator <<(std::ostream& stream, const Sample<R, A...>&
      sample) {
    stream << '(' << sample.m_result;
    std::apply([&](const auto&... arg) {
      ((stream << ", " << arg), ...);
    }, sample.m_arguments);
    stream << ')';
    return stream;
  }

  template<typename R, typename... A>
  std::istream& operator >>(std::istream& stream, Sample<R, A...>& sample) {
    auto parameter_reader = [&](auto& destination, int characters_to_ignore) {
      if(!stream.good()) {
        return;
      }
      stream.ignore(characters_to_ignore);
      if(!stream.good()) {
        return;
      }
      stream >> destination;
    };
    parameter_reader(sample.m_result, 1);
    std::apply([&](auto&... arg) {
      (parameter_reader(arg, 2), ...);
    }, sample.m_arguments);
    if(stream.good()) {
      stream.ignore(1);
    }
    return stream;
  }

  template<typename A>
  template<typename Func>
  void ArgumentVisitor<A>::visit(Func&& func, Arguments& tuple) {
    std::apply([&](auto&&... args) {
      auto i = std::size_t(0);
      (func(args, i++), ...);
    }, tuple);
  }

  template<typename A>
  template<typename Func>
  void ArgumentVisitor<A>::visit(Func&& func, const Arguments& tuple) {
    std::apply([&](auto&&... args) {
      auto i = std::size_t(0);
      (func(args, i++), ...);
    }, tuple);
  }

  template<typename A>
  constexpr std::size_t ArgumentVisitor<A>::size(const Arguments& tuple) {
    return std::tuple_size_v<Arguments>;
  }
}

#endif
