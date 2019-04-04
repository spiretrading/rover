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
}

#endif
