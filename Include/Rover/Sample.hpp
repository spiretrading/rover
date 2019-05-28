#ifndef ROVER_SAMPLE_HPP
#define ROVER_SAMPLE_HPP
#include <iostream>
#include <sstream>
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
  /*!
    \param stream The output stream.
    \param sample The sample to serialize.
  */
  template<typename R, typename... A>
  std::ostream& operator <<(std::ostream& stream, const Sample<R, A...>&
    sample);

  //! Deserializes a Sample from the comma-separated format.
  /*!
    \param stream The input stream.
    \param result The resulting sample.
  */
  template<typename R, typename... A>
  std::istream& operator >>(std::istream& stream, Sample<R, A...>& result);

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

  template<typename Argument>
  bool get_next_cvs_field(std::istream& stream, Argument& result) {
    if(!stream.good()) {
      return false;
    }
    auto output_string_stream = std::ostringstream();
    if(stream.peek() == '"') {
      stream.ignore();
      if(!stream.good()) {
        return false;
      }
      while(true) {
        auto c = stream.get();
        if(!std::istream::traits_type::not_eof(c)) {
          break;
        }
        auto next_character = std::istream::traits_type::to_char_type(c);
        if(next_character == '"') {
          c = stream.get();
          if(!std::istream::traits_type::not_eof(c)) {
            break;
          }
          auto following_character = std::istream::traits_type::to_char_type(
            c);
          if(following_character == '\0' || following_character == ',' ||
              following_character == '\n') {
            break;
          } else if(following_character == '"') {
            output_string_stream << '"';
          } else {
            return false;
          }
        } else {
          output_string_stream << next_character;
        }
      }
    } else {
      while(true) {
        auto c = stream.get();
        if(!std::istream::traits_type::not_eof(c)) {
          break;
        }
        auto next_character = std::istream::traits_type::to_char_type(c);
        if(next_character != '\0' && next_character != ',' &&
            next_character != '\n') {
          output_string_stream << next_character;
        }
        else {
          break;
        }
      }
    }
    if(stream || stream.eof()) {
      auto input_string_stream = std::istringstream(
        output_string_stream.str());
      auto argument = Argument();
      input_string_stream >> argument;
      if(input_string_stream || input_string_stream.eof()) {
        result = std::move(argument);
        return true;
      }
    }
    return false;
  }

  template<typename Argument>
  void put_next_csv_field(const Argument& argument, std::ostream& stream) {
    auto string_stream = std::ostringstream();
    string_stream << argument;
    auto field = string_stream.str();
    if(std::find_if(field.begin(), field.end(), [](auto c) {
         return c == ',' || c == '"' || c == '\n';
       }) == field.end()) {
      stream << field;
      return;
    }
    stream.put('"');
    for(auto c : field) {
      if(c == '"') {
        stream.put('"');
      }
      stream.put(c);
    }
    stream.put('"');
  }
  
  template<typename R, typename... A>
  std::ostream& operator <<(std::ostream& stream, const Sample<R, A...>&
      sample) {
    put_next_csv_field(sample.m_result, stream);
    auto add_field = [&](const auto& arg) {
      stream << ',';
      put_next_csv_field(arg, stream);
    };
    std::apply([&](const auto&... args) {
      (add_field(args), ...);
    }, sample.m_arguments);
    return stream;
  }

  template<typename R, typename... A>
  std::istream& operator >>(std::istream& stream, Sample<R, A...>& result) {
    auto sample = Sample<R, A...>();
    auto parameter_reader = [&](auto& parameter) {
      if(stream.good()) {
        return get_next_cvs_field(stream, parameter);
      }
      return false;
    };
    parameter_reader(sample.m_result);
    if(std::apply([&](auto&... arg) {
         return (parameter_reader(arg) && ...);
       }, sample.m_arguments)) {
      if(stream || stream.eof()) {
        result = std::move(sample);
      }
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
