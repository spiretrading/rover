#ifndef ROVER_TRIAL_VIEW_HPP
#define ROVER_TRIAL_VIEW_HPP
#include <functional>
#include <type_traits>

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

    private:
      using GetPtr = std::function<const Sample* (std::size_t)>;

    public:

      //! Random access constant iterator.
      class ConstIterator {
        public:

          //! Creates a constant iterator.
          /*
            \param get_ptr Reference to TrialView::m_get_ptr.
            \param offset Offset from t.begin().
          */
          ConstIterator(const GetPtr& get_ptr, std::size_t offset);

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
          ConstIterator operator +(std::ptrdiff_t offset) const;

          //! Returns an iterator that is the result of moving this back.
          ConstIterator operator -(std::ptrdiff_t offset) const;

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
          const GetPtr* m_get_ptr;
          std::size_t m_offset;
      };

      //! Creates a TrialView for a trial.
      template<typename Trial>
      TrialView(const Trial& t);

      //! Creates a TrialView for a trial.
      template<typename Trial, std::enable_if_t<!std::is_lvalue_reference_v<
        Trial>>* = nullptr>
      TrialView(Trial&& t);

      //! Returns an iterator to the beginning of the trial.
      ConstIterator begin() const;

      //! Returns an iterator to the end of the trial.
      ConstIterator end() const;

      //! Returns the size of the trial.
      std::size_t size() const;

      //! Returns a reference to the element at the given index.
      const Sample& operator [](std::size_t index) const;

    private:
      std::size_t m_size;
      GetPtr m_get_ptr;
      ConstIterator m_begin;
      ConstIterator m_end;
  };

  template<typename Trial>
  TrialView(const Trial&) -> TrialView<typename Trial::Sample>;

  template<typename S>
  TrialView<S>::ConstIterator::ConstIterator(const GetPtr& get_ptr,
      std::size_t offset)
    : m_get_ptr(&get_ptr),
      m_offset(offset) {}

  template<typename S>
  typename const TrialView<S>::Sample&
      TrialView<S>::ConstIterator::operator *() const {
    return *(*m_get_ptr)(m_offset);
  }

  template<typename S>
  typename const TrialView<S>::Sample*
      TrialView<S>::ConstIterator::operator ->() const {
    return (*m_get_ptr)(m_offset);
  }

  template<typename S>
  typename const TrialView<S>::Sample&
      TrialView<S>::ConstIterator::operator [](std::ptrdiff_t offset) const {
    return *(*m_get_ptr)(m_offset + offset);
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
      TrialView<S>::ConstIterator::operator +(std::ptrdiff_t offset) const {
    auto result = *this;
    result += offset;
    return result;
  }

  template<typename S>
  typename TrialView<S>::ConstIterator
      TrialView<S>::ConstIterator::operator -(std::ptrdiff_t offset) const {
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
    : m_size(t.size()),
      m_get_ptr([begin = t.begin()] (std::size_t offset) {
        return &(*(begin + offset));
      }),
      m_begin(m_get_ptr, 0),
      m_end(m_get_ptr, m_size) {}

  template<typename T>
  template<typename Trial, std::enable_if_t<!std::is_lvalue_reference_v<
    Trial>>*>
  TrialView<T>::TrialView(Trial&& t)
    : m_size(t.size()),
      m_get_ptr([t = std::move(t)](std::size_t offset) {
        return &(*(t.begin() + offset));
      }),
      m_begin(m_get_ptr, 0),
      m_end(m_get_ptr, m_size) {}

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
