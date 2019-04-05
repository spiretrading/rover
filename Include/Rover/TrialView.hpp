#ifndef ROVER_TRIAL_VIEW_HPP
#define ROVER_TRIAL_VIEW_HPP
#include <functional>

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

      //! Random access constant iterator.
      class ConstIterator {
        public:

          //! Creates a constant iterator.
          /*
            \param t Trial to read.
            \param offset Offset from t.begin().
          */
          template<typename Trial>
          ConstIterator(const Trial& t, std::ptrdiff_t offset);

          //! Increments the iterator.
          ConstIterator& operator ++();

          //! Increments the iterator.
          ConstIterator operator ++(int);

          //! Decrements the iterator.
          ConstIterator& operator --();

          //! Decrements the iterator.
          ConstIterator operator --(int);

          //! Advances the iterator.
          ConstIterator& operator +=(std::ptrdiff_t offset);

          //! Moves the iterator back.
          ConstIterator& operator -=(std::ptrdiff_t offset);

          //! Returns an iterator that is the result of advancing this.
          ConstIterator operator +(std::ptrdiff_t offset);

          //! Returns an iterator that is the result of moving this back.
          ConstIterator operator -(std::ptrdiff_t offset);

          //! Returns the offset between two iterators.
          std::ptrdiff_t operator -(ConstIterator other) const;

          //! Checks whether two iterators point to the same Sample.
          bool operator ==(ConstIterator other) const;

          //! Checks whether two iterators do not point to the same Sample.
          bool operator !=(ConstIterator other) const;

          //! Checks whether the Sample pointed to by the first iterator is to
          //! the left of that pointed to by the second iterator.
          bool operator <(ConstIterator other) const;

          //! Checks whether the Sample pointed to by the first iterator is not
          //! to the right of that pointed to by the second iterator.
          bool operator <=(ConstIterator other) const;

          //! Checks whether the Sample pointed to by the first iterator is to
          //! the right of that pointed to by the second iterator.
          bool operator >(ConstIterator other) const;

          //! Checks whether the Sample pointed to by the first iterator is not
          //! to the left of that pointed to by the second iterator.
          bool operator >=(ConstIterator other) const;

          //! Returns a reference to the Sample the iterator points to.
          const Sample& operator *() const;

          //! Returns a pointer to the Sample the iterator points to.
          const Sample* operator ->() const;

          //! Returns a reference to the Sample the iterator would point to
          //! if it was advanced by the offset.
          const Sample& operator [](std::ptrdiff_t offset) const;

        private:
          std::ptrdiff_t m_offset;
          std::function<const Sample* (std::ptrdiff_t)> m_get_ptr;
      };

      //! Creates a TrialView for a trial.
      template<typename Trial>
      TrialView(const Trial& t);

      //! Returns an iterator to the beginning of the trial.
      ConstIterator begin() const;

      //! Returns an iterator to the end of the trial.
      ConstIterator end() const;

      //! Returns the size of the trial.
      std::size_t size() const;

      //! Returns a reference to the element at the given index.
      const Sample& operator [](std::size_t index) const;

    private:
      ConstIterator m_begin;
      ConstIterator m_end;
      std::size_t m_size;
  };

  template<typename Trial>
  TrialView(const Trial&) -> TrialView<typename Trial::Sample>;

  template<typename S>
  template<typename Trial>
  TrialView<S>::ConstIterator::ConstIterator(const Trial& t,
      std::ptrdiff_t offset)
    : m_offset(offset),
      m_get_ptr([begin = t.begin()] (std::ptrdiff_t offset)
      {
        return &(*(begin + offset));
      }) {}

  template<typename S>
  typename const TrialView<S>::Sample&
      TrialView<S>::ConstIterator::operator *() const {
    return *m_get_ptr(m_offset);
  }

  template<typename S>
  typename const TrialView<S>::Sample*
      TrialView<S>::ConstIterator::operator ->() const {
    return m_get_ptr(m_offset);
  }

  template<typename S>
  typename const TrialView<S>::Sample&
      TrialView<S>::ConstIterator::operator [](std::ptrdiff_t offset) const {
    return *m_get_ptr(m_offset + offset);
  }

  template<typename S>
  bool TrialView<S>::ConstIterator::operator ==(ConstIterator other) const {
    return m_offset == other.m_offset;
  }

  template<typename S>
  bool TrialView<S>::ConstIterator::operator !=(ConstIterator other) const {
    return m_offset != other.m_offset;
  }

  template<typename S>
  bool TrialView<S>::ConstIterator::operator <(ConstIterator other) const {
    return m_offset < other.m_offset;
  }

  template<typename S>
  bool TrialView<S>::ConstIterator::operator <=(ConstIterator other) const {
    return m_offset <= other.m_offset;
  }

  template<typename S>
  bool TrialView<S>::ConstIterator::operator >(ConstIterator other) const {
    return m_offset > other.m_offset;
  }

  template<typename S>
  bool TrialView<S>::ConstIterator::operator >=(ConstIterator other) const {
    return m_offset >= other.m_offset;
  }

  template<typename S>
  typename TrialView<S>::ConstIterator&
      TrialView<S>::ConstIterator::operator ++() {
    ++m_offset;
    return *this;
  }

  template<typename S>
  typename TrialView<S>::ConstIterator
      TrialView<S>::ConstIterator::operator ++(int) {
    ++m_offset;
    return *this;
  }

  template<typename S>
  typename TrialView<S>::ConstIterator&
      TrialView<S>::ConstIterator::operator --() {
    --m_offset;
    return *this;
  }

  template<typename S>
  typename TrialView<S>::ConstIterator
      TrialView<S>::ConstIterator::operator --(int) {
    --m_offset;
    return *this;
  }

  template<typename S>
  typename TrialView<S>::ConstIterator&
      TrialView<S>::ConstIterator::operator +=(std::ptrdiff_t offset) {
    m_offset += offset;
    return *this;
  }

  template<typename S>
  typename TrialView<S>::ConstIterator&
      TrialView<S>::ConstIterator::operator -=(std::ptrdiff_t offset) {
    m_offset -= offset;
    return *this;
  }

  template<typename S>
  typename TrialView<S>::ConstIterator
      TrialView<S>::ConstIterator::operator +(std::ptrdiff_t offset) {
    auto result = *this;
    result += offset;
    return result;
  }

  template<typename S>
  typename TrialView<S>::ConstIterator
      TrialView<S>::ConstIterator::operator -(std::ptrdiff_t offset) {
    auto result = *this;
    result -= offset;
    return result;
  }

  template<typename S>
  std::ptrdiff_t TrialView<S>::ConstIterator::operator -(
      ConstIterator other) const {
    return m_offset - other.m_offset;
  }

  template<typename T>
  template<typename Trial>
  TrialView<T>::TrialView(const Trial& t)
    : m_begin(ConstIterator(t, std::ptrdiff_t(0))),
      m_end(ConstIterator(t, static_cast<std::ptrdiff_t>(t.size()))),
      m_size(t.size()) {}

  template<typename T>
  typename TrialView<T>::ConstIterator TrialView<T>::begin() const {
    return m_begin;
  }

  template<typename T>
  typename TrialView<T>::ConstIterator TrialView<T>::end() const {
    return m_end;
  }

  template<typename T>
  std::size_t TrialView<T>::size() const {
    return m_size;
  }

  template<typename T>
  typename const TrialView<T>::Sample& TrialView<T>::operator [](std::size_t
      index) const {
    return m_begin[static_cast<std::ptrdiff_t>(index)];
  }
}

#endif
