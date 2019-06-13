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
      using SampleGetter = G;

      //! Type used by the algorithm.
      using Type = typename std::invoke_result_t<SampleGetter,
        std::size_t>::Result;

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
      template<typename SampleGetterFwd>
      ScalarView(SampleGetterFwd&& get, std::size_t size);

      //! Constructs a ScalarView using another ScalarView parameterized with a
      //! different getter function but returning the same Sample type.
      /*!
        \param view ScalarView returning the same Sample type.
      */
      template<typename OtherScalarView, std::enable_if_t<
        !std::is_convertible_v<std::decay_t<OtherScalarView>, ScalarView>>* =
        nullptr>
      ScalarView(OtherScalarView&& view);

      //! Returns an input iterator to the beginning of the trial.
      Iterator begin() const;

      //! Returns an input iterator to the end of the trial.
      Iterator end() const;

      //! Returns ith adapted sample.
      Sample operator [](std::size_t i) const;

      //! Returns the number of the samples.
      std::size_t size() const;

    private:
      std::size_t m_size;
      SampleGetter m_getter;
  };

  template<typename G>
  ScalarView(G&&, std::size_t) -> ScalarView<std::decay_t<G>>;

  template<typename G>
  template<typename SampleGetterFwd>
  ScalarView<G>::ScalarView(SampleGetterFwd&& getter, std::size_t size)
    : m_size(size),
      m_getter(std::forward<SampleGetterFwd>(getter)) {}

  template<typename G>
  template<typename OtherScalarView, std::enable_if_t<
    !std::is_convertible_v<std::decay_t<OtherScalarView>, ScalarView<G>>>*>
  ScalarView<G>::ScalarView(OtherScalarView&& view)
    : m_size(view.size()),
      m_getter([view = std::forward<OtherScalarView>(view)](std::size_t
          index) {
        return view[index];
      }) {}

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
    return m_getter(i);
  }

  template<typename G>
  std::size_t ScalarView<G>::size() const {
    return m_size;
  }
}

#endif
