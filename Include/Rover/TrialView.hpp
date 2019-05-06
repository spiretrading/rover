#ifndef ROVER_TRIAL_VIEW_HPP
#define ROVER_TRIAL_VIEW_HPP
#include <functional>
#include "TrialIterator.hpp"

namespace Rover {

  //! Read-only random access reader for any Trial for a given Sample.
  /*
    \tparam S The type of the samples.
  */
  template<typename S>
  class TrialView {
    public:

      //! The type of the samples stored in the Trial.
      using Sample = S;

      //! The type of the iterator.
      using Iterator = TrialIterator<TrialView>;

      //! Creates a TrialView for a trial.
      template<typename Trial>
      TrialView(const Trial& t);

      //! Returns an iterator to the beginning of the trial.
      Iterator begin() const;

      //! Returns an iterator to the end of the trial.
      Iterator end() const;

      //! Returns the size of the trial.
      std::size_t size() const;

      //! Returns a reference to the element at the given index.
      const Sample& operator [](std::size_t index) const;

    private:
      using SampleGetter = std::function<const Sample& (std::size_t)>;

      std::size_t m_size;
      SampleGetter m_getter;
  };

  template<typename Trial>
  TrialView(const Trial&) -> TrialView<typename Trial::Sample>;

  template<typename T>
  template<typename Trial>
  TrialView<T>::TrialView(const Trial& t)
    : m_size(t.size()),
      m_getter([begin = t.begin()](std::size_t offset) mutable -> const
          Sample& {
        return begin[offset];
      }) {}

  template<typename T>
  typename TrialView<T>::Iterator TrialView<T>::begin() const {
    return Iterator(*this, 0);
  }

  template<typename T>
  typename TrialView<T>::Iterator TrialView<T>::end() const {
    return Iterator(*this, m_size);
  }

  template<typename T>
  std::size_t TrialView<T>::size() const {
    return m_size;
  }

  template<typename T>
  const typename TrialView<T>::Sample& TrialView<T>::operator [](std::size_t
      index) const {
    return m_getter(index);
  }
}

#endif
