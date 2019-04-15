#ifndef ROVER_SCALAR_VIEW_HPP
#define ROVER_SCALAR_VIEW_HPP
#include <functional>
#include <optional>
#include <vector>

namespace Rover {

  //! Interface to pass adapted Trial Samples to typed Algorithms.
  /*!
    \tparam T CompType used by the algorithm.
  */
  template<typename T>
  class ScalarView {
    public:

      //! CompType used by the algorithm.
      using Type = T;

      //! Adapted sample.
      struct Sample {

        //! The type of the result.
        using Result = Type;

        //! The type of the arguments.
        using Arguments = std::vector<Type>;

        //! The result of the trial's function.
        Result m_result;

        //! The arguments passed to the trial's function.
        Arguments m_arguments;
      };

      //! Constant input iterator.
      class ConstIterator {
        public:
          
          //! Checks whether two iterators point to the same Sample.
          bool operator ==(ConstIterator other) const;

          //! Checks whether two iterators do not point to the same Sample.
          bool operator !=(ConstIterator other) const;

          //! Returns a reference to the Sample the iterator points to.
          const Sample& operator *() const;

          //! Returns a pointer to the Sample the iterator points to.
          const Sample* operator ->() const;

          //! Increments the iterator.
          ConstIterator& operator ++();

          //! Increments the iterator.
          ConstIterator operator ++(int);

        private:
          friend class ScalarView;

          const ScalarView* m_reader;
          std::size_t m_index;
          std::optional<Sample> m_sample;

          ConstIterator(const ScalarView* reader, std::size_t index);
          std::optional<Sample> retrieve_sample() const;
      };

      //! Creates a ScalarView.
      /*!
        \param get The function taking an index i and returning the ith adapted
                   sample.
        \param size The total number of samples.
      */
      template<typename GetFwd>
      ScalarView(GetFwd&& get, std::size_t size);

      //! Returns an input iterator to the beginning of the trial.
      ConstIterator begin() const;

      //! Returns an input iterator to the end of the trial.
      ConstIterator end() const;

      //! Returns ith adapted sample.
      Sample operator [](std::size_t i) const;

      //! Returns the number of the samples.
      std::size_t size() const;

    private:
      std::function<Sample (std::size_t)> m_get;
      std::size_t m_size;
  };

  template<typename T>
  bool ScalarView<T>::ConstIterator::operator ==(ConstIterator other) const {
    return m_reader == other.m_reader && m_index == other.m_index;
  }

  template<typename T>
  bool ScalarView<T>::ConstIterator::operator !=(ConstIterator other) const {
    return !(*this == other);
  }

  template<typename T>
  typename const ScalarView<T>::Sample&
      ScalarView<T>::ConstIterator::operator *() const {
    return *m_sample;
  }

  template<typename T>
  typename const ScalarView<T>::Sample*
      ScalarView<T>::ConstIterator::operator ->() const {
    return std::addressof(m_sample);
  }

  template<typename T>
  typename ScalarView<T>::ConstIterator&
      ScalarView<T>::ConstIterator::operator ++() {
    ++m_index;
    m_sample = retrieve_sample();
    return *this;
  }

  template<typename T>
  typename ScalarView<T>::ConstIterator 
      ScalarView<T>::ConstIterator::operator ++(int) {
    auto copy = *this;
    ++m_index;
    m_sample = retrieve_sample();
    return copy;
  }
  
  template<typename T>
  ScalarView<T>::ConstIterator::ConstIterator(const ScalarView* reader,
      std::size_t index)
    : m_reader(reader),
      m_index(index),
      m_sample(retrieve_sample()) {}

  template<typename T>
  std::optional<typename ScalarView<T>::Sample>
      ScalarView<T>::ConstIterator::retrieve_sample() const {
    if(m_index < m_reader->size()) {
      return (*m_reader)[m_index];
    } else {
      return std::nullopt;
    }
  }

  template<typename T>
  template<typename GetFwd>
  ScalarView<T>::ScalarView(GetFwd&& get, std::size_t size)
    : m_get(std::forward<GetFwd>(get)),
      m_size(size) {}

  template<typename T>
  typename ScalarView<T>::ConstIterator ScalarView<T>::begin() const {
    return ConstIterator(this, 0);
  }

  template<typename T>
  typename ScalarView<T>::ConstIterator ScalarView<T>::end() const {
    return ConstIterator(this, m_size);
  }

  template<typename T>
  typename ScalarView<T>::Sample ScalarView<T>::operator [](
      std::size_t i) const {
    return m_get(i);
  }

  template<typename T>
  std::size_t ScalarView<T>::size() const {
    return m_size;
  }
}

#endif