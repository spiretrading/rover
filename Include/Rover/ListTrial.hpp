#ifndef ROVER_LIST_TRIAL_HPP
#define ROVER_LIST_TRIAL_HPP
#include <vector>
#include "Rover/TrialIterator.hpp"

namespace Rover {

  //! Stores samples as an array in RAM
  /*!
    \tparam S The type of the samples.
  */
  template<typename S>
  class ListTrial {
    public:
      
      //! The type of stored samples.
      using Sample = S;

      //! The type of the constant iterator.
      using Iterator = TrialIterator<ListTrial>;
  
      //! Increases the capacity of the underlying array.
      void reserve(std::size_t capacity);

      //! Inserts a sample to this trial.
      void insert(const Sample& s);
  
      //! Inserts a sample to this trial.
      void insert(Sample&& s);
  
      //! Inserts all samples from a collection via iterators to this one.
      template<typename Begin, typename End>
      void insert(Begin b, End e);
  
      //! Returns a constant iterator to the first sample
      Iterator begin() const;

      //! Returns a constant iterator to the past-the-end sample
      Iterator end() const;

      //! Number of samples in this trial.
      std::size_t size() const;

      //! Number of samples that can be stored without re-allocations
      std::size_t capacity() const;
  
      //! Returns a sample.
      const Sample& operator [](std::size_t index) const;

    private:
      std::vector<Sample> m_samples;
  };

  template<typename S>
  void ListTrial<S>::reserve(std::size_t capacity) {
    m_samples.reserve(capacity);
  }

  template<typename S>
  void ListTrial<S>::insert(const Sample& s) {
    m_samples.push_back(s);
  }

  template<typename S>
  void ListTrial<S>::insert(Sample&& s) {
    m_samples.push_back(std::move(s));
  }

  template<typename S>
  template<typename Begin, typename End>
  void ListTrial<S>::insert(Begin begin, End end) {
    std::copy(begin, end, std::back_inserter(m_samples));
  }

  template<typename S>
  typename ListTrial<S>::Iterator ListTrial<S>::begin() const {
    return Iterator(*this, 0);
  }

  template<typename S>
  typename ListTrial<S>::Iterator ListTrial<S>::end() const {
    return Iterator(*this, size());
  }

  template<typename S>
  std::size_t ListTrial<S>::size() const {
    return m_samples.size();
  }

  template<typename S>
  std::size_t ListTrial<S>::capacity() const {
    return m_samples.capacity();
  }

  template<typename S>
  const typename ListTrial<S>::Sample& ListTrial<S>::operator [](
      std::size_t index) const {
    return m_samples[index];
  }
}

#endif
