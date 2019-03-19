#ifndef ROVER_SAMPLE_HPP
#define ROVER_SAMPLE_HPP
#include <tuple>

namespace Rover {

  //! Represents a single execution of a function of a set of parameters.
  /*!
    \tparam R The type of the result.
    \tparam P The types of the parameters.
  */
  template<typename R, typename... P>
  struct Sample {

    //! The type of the result.
    using Result = R;

    //! The type of the parameters.
    using Parameters = std::tuple<P...>;

    //! The result of the function.
    Result m_result;

    //! The arguments passed to the function.
    Parameters m_arguments;
  };
}

#endif
