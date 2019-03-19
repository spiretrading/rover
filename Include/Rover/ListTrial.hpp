#ifndef ROVER_LIST_TRIAL_HPP
#define ROVER_LIST_TRIAL_HPP
#include <vector>

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
      using ConstIterator = typename std::vector<Sample>::const_iterator;
  
      //! Increases the capacity of the underlying array.
      void reserve(std::size_t capacity);

      //! Inserts a sample to this trial.
      void insert(const Sample& s);
  
      //! Inserts a sample to this trial.
      void insert(Sample&& s);

      //! Inserts all samples from another trial to this one.
      void insert(const ListTrial& t);

      //! Inserts all samples from another trial to this one.
      void insert(ListTrial&& t);
  
      //! Inserts all samples from a collection via iterators to this one.
      template<typename Begin, typename End>
      void insert(Begin b, End e);
  
      //! Returns a constant iterator to the first sample
      ConstIterator begin() const;

      //! Returns a constant iterator to the past-the-end sample
      ConstIterator end() const;

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
  void ListTrial<S>::insert(const ListTrial& t) {
    insert(t.m_samples.begin(), t.m_samples.end());
  }

  template<typename S>
  void ListTrial<S>::insert(ListTrial&& t) {
    insert(std::make_move_iterator(t.m_samples.begin()),
      std::make_move_iterator(t.m_samples.end()));
  }

  template<typename S>
  template<typename Begin, typename End>
  void ListTrial<S>::insert(Begin begin, End end) {
    m_samples.insert(m_samples.end(), begin, end);
  }

  template<typename S>
  typename ListTrial<S>::ConstIterator ListTrial<S>::begin() const {
    return m_samples.begin();
  }

  template<typename S>
  typename ListTrial<S>::ConstIterator ListTrial<S>::end() const {
    return m_samples.end();
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
