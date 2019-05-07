#ifndef ROVER_SCALAR_VIEW_HPP
#define ROVER_SCALAR_VIEW_HPP
#include <functional>
#include <optional>
#include <type_traits>
#include <vector>
#include "Rover/TrialIterator.hpp"

namespace Rover {

  //! Representation of a sample using a single numeric type.
  template<typename T>
  struct ScalarSample {

    //! The type of the result.
    using Result = T;

    //! The type of the arguments.
    using Arguments = std::vector<T>;

    //! The result of the trial's function.
    Result m_result;

    //! The arguments passed to the trial's function.
    Arguments m_arguments;
  };

  //! Interface to pass adapted Trial Samples to typed Algorithms.
  /*!
    \tparam G The type of a function returning adapted samples by index.
  */
  template<typename G>
  class ScalarView {
    public:

      //! The type of a function returning adapted samples by index.
      using Get = G;

      //! Type used by the algorithm.
      using Type = typename std::invoke_result_t<Get, std::size_t>::Result;

      //! The type representing a sample.
      using Sample = ScalarSample<Type>;

      //! The type of the iterator.
      using Iterator = TrialIterator<ScalarView>;

      //! Creates a ScalarView.
      /*!
        \param get The function taking an index i and returning the ith adapted
                   sample.
        \param size The total number of samples.
      */
      template<typename GetFwd>
      ScalarView(GetFwd&& get, std::size_t size);

      //! Returns an input iterator to the beginning of the trial.
      Iterator begin() const;

      //! Returns an input iterator to the end of the trial.
      Iterator end() const;

      //! Returns ith adapted sample.
      Sample operator [](std::size_t i) const;

      //! Returns the number of the samples.
      std::size_t size() const;

    private:
      Get m_get;
      std::size_t m_size;
  };

  template<typename G>
  ScalarView(G&&, std::size_t) -> ScalarView<std::decay_t<G>>;

  template<typename G>
  template<typename GetFwd>
  ScalarView<G>::ScalarView(GetFwd&& get, std::size_t size)
    : m_get(std::forward<GetFwd>(get)),
      m_size(size) {}

  template<typename G>
  typename ScalarView<G>::Iterator ScalarView<G>::begin() const {
    return Iterator(*this, 0);
  }

  template<typename G>
  typename ScalarView<G>::Iterator ScalarView<G>::end() const {
    return Iterator(*this, m_size);
  }

  template<typename G>
  typename ScalarView<G>::Sample ScalarView<G>::operator [](
      std::size_t i) const {
    return m_get(i);
  }

  template<typename G>
  std::size_t ScalarView<G>::size() const {
    return m_size;
  }
}

#endif
