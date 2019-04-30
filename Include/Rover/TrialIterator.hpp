#ifndef ROVER_TRIAL_ITERATOR_HPP
#define ROVER_TRIAL_ITERATOR_HPP
#include <iterator>
#include <optional>
#include <type_traits>

namespace Rover {

  //! Iterator over a Trial.
  template<typename, typename = void>
  class TrialIterator;

  //! Type traits to check whether a Trial returns Samples by reference.
  template<typename T>
  inline constexpr bool returns_sample_by_reference_v = std::is_reference_v<
    decltype(std::declval<T>()[std::declval<std::size_t>()])>;

  //! Type traits to check whether a Trial returns Samples by copy.
  template<typename T>
  inline constexpr bool returns_sample_by_copy_v =
    !returns_sample_by_reference_v<T>;

  //! Iterator over a Trial that returns Samples by copy.
  /*
    \tparam T The type of the Trial.
    \details Relies on the operator []'s return type to identify the
             return-by-copy. Returned references and pointers are only valid
             as long as the iterator is alive and another de-referencing
             operation is executed.
  */
  template<typename T>
  class TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<T>>> {
    public:

      //! The type of the trial.
      using Trial = T;

      //! The type of the samples.
      using Sample = typename Trial::Sample;

      //! Iterator category for compliance with STL algorithms.
      using iterator_category = std::input_iterator_tag;

      //! Iterator value type for compliance with STL algorithms.
      using value_type = Sample;

      //! Iterator difference type for compliance with STL algorithms.
      using difference_type = std::ptrdiff_t;

      //! Iterator pointer type for compliance with STL algorithms.
      using pointer = const Sample*;

      //! Iterator reference type for compliance with STL algorithms.
      using reference = const Sample&;

      //! Increments the iterator.
      TrialIterator& operator ++();

      //! Increments the iterator.
      TrialIterator operator ++(int);

      //! Decrements the iterator.
      TrialIterator& operator --();

      //! Decrements the iterator.
      TrialIterator operator --(int);

      //! Advances the iterator.
      TrialIterator& operator +=(std::ptrdiff_t offset);

      //! Moves the iterator back.
      TrialIterator& operator -=(std::ptrdiff_t offset);

      //! Returns an iterator that is the result of advancing this.
      TrialIterator operator +(std::ptrdiff_t offset) const;

      //! Returns an iterator that is the result of moving this back.
      TrialIterator operator -(std::ptrdiff_t offset) const;

      //! Returns the offset between two iterators.
      std::ptrdiff_t operator -(const TrialIterator& other) const;

      //! Checks whether two iterators point to the same Sample.
      bool operator ==(const TrialIterator& other) const;

      //! Checks whether two iterators do not point to the same Sample.
      bool operator !=(const TrialIterator& other) const;

      //! Checks whether the Sample pointed to by the first iterator is to
      //! the left of that pointed to by the second iterator.
      bool operator <(const TrialIterator& other) const;

      //! Checks whether the Sample pointed to by the first iterator is not
      //! to the right of that pointed to by the second iterator.
      bool operator <=(const TrialIterator& other) const;

      //! Checks whether the Sample pointed to by the first iterator is to
      //! the right of that pointed to by the second iterator.
      bool operator >(const TrialIterator& other) const;

      //! Checks whether the Sample pointed to by the first iterator is not
      //! to the left of that pointed to by the second iterator.
      bool operator >=(const TrialIterator& other) const;

      //! Returns a reference to the Sample the iterator points to.
      /*
        \details The reference is valid as long as the iterator is alive, or
                 until another de-referencing operation is executed.
      */
      const Sample& operator *();

      //! Returns a pointer to the Sample the iterator points to.
      /*
        \details The pointer is valid as long as the iterator is alive, or
                 until another de-referencing operation is executed.
      */
      const Sample* operator ->();

      //! Returns a reference to the Sample the iterator would point to
      //! if it was advanced by the offset.
      /*
        \details The reference is valid as long as the iterator is alive, or
                 until another de-referencing operation is executed.
      */
      const Sample& operator [](std::ptrdiff_t offset);

    private:
      friend Trial;

      const Trial* m_trial;
      std::size_t m_offset;
      std::optional<Sample> m_sample;

      TrialIterator(const Trial& trial, std::size_t offset);
  };

  //! Iterator over a Trial that returns Samples by reference.
  /*
    \tparam T The type of the Trial.
    \details Relies on the operator []'s return type to identify the
             return-by-reference.
  */
  template<typename T>
  class TrialIterator<T, std::enable_if_t<returns_sample_by_reference_v<T>>> {
    public:

      //! The type of the trial.
      using Trial = T;

      //! The type of the samples.
      using Sample = typename Trial::Sample;

      //! Iterator category for compliance with STL algorithms.
      using iterator_category = std::random_access_iterator_tag;

      //! Iterator value type for compliance with STL algorithms.
      using value_type = Sample;

      //! Iterator difference type for compliance with STL algorithms.
      using difference_type = std::ptrdiff_t;

      //! Iterator pointer type for compliance with STL algorithms.
      using pointer = const Sample*;

      //! Iterator reference type for compliance with STL algorithms.
      using reference = const Sample&;

      //! Increments the iterator.
      TrialIterator& operator ++();

      //! Increments the iterator.
      TrialIterator operator ++(int);

      //! Decrements the iterator.
      TrialIterator& operator --();

      //! Decrements the iterator.
      TrialIterator operator --(int);

      //! Advances the iterator.
      TrialIterator& operator +=(std::ptrdiff_t offset);

      //! Moves the iterator back.
      TrialIterator& operator -=(std::ptrdiff_t offset);

      //! Returns an iterator that is the result of advancing this.
      TrialIterator operator +(std::ptrdiff_t offset) const;

      //! Returns an iterator that is the result of moving this back.
      TrialIterator operator -(std::ptrdiff_t offset) const;

      //! Returns the offset between two iterators.
      std::ptrdiff_t operator -(TrialIterator other) const;

      //! Checks whether two iterators point to the same Sample.
      bool operator ==(TrialIterator other) const;

      //! Checks whether two iterators do not point to the same Sample.
      bool operator !=(TrialIterator other) const;

      //! Checks whether the Sample pointed to by the first iterator is to
      //! the left of that pointed to by the second iterator.
      bool operator <(TrialIterator other) const;

      //! Checks whether the Sample pointed to by the first iterator is not
      //! to the right of that pointed to by the second iterator.
      bool operator <=(TrialIterator other) const;

      //! Checks whether the Sample pointed to by the first iterator is to
      //! the right of that pointed to by the second iterator.
      bool operator >(TrialIterator other) const;

      //! Checks whether the Sample pointed to by the first iterator is not
      //! to the left of that pointed to by the second iterator.
      bool operator >=(TrialIterator other) const;

      //! Returns a reference to the Sample the iterator points to.
      const Sample& operator *() const;

      //! Returns a pointer to the Sample the iterator points to.
      const Sample* operator ->() const;

      //! Returns a reference to the Sample the iterator would point to
      //! if it was advanced by the offset.
      const Sample& operator [](std::ptrdiff_t offset) const;

    private:
      friend Trial;

      const Trial* m_trial;
      std::size_t m_offset;

      TrialIterator(const Trial& trial, std::size_t offset);
  };

  template<typename T>
  TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<T>>>&
      TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<
      T>>>::operator ++() {
    ++m_offset;
    return *this;
  }

  template<typename T>
  TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<T>>>
      TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<
      T>>>::operator ++(int) {
    auto copy = *this;
    ++m_offset;
    return copy;
  }

  template<typename T>
  TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<T>>>&
      TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<
      T>>>::operator --() {
    --m_offset;
    return *this;
  }

  template<typename T>
  TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<T>>>
      TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<
      T>>>::operator --(int) {
    auto copy = *this;
    --m_offset;
    return copy;
  }

  template<typename T>
  TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<T>>>&
      TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<
      T>>>::operator +=(std::ptrdiff_t offset) {
    m_offset += offset;
    return *this;
  }

  template<typename T>
  TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<T>>>&
      TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<
      T>>>::operator -=(std::ptrdiff_t offset) {
    m_offset -= offset;
    return *this;
  }

  template<typename T>
  TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<T>>>
      TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<
      T>>>::operator +(std::ptrdiff_t offset) const {
    auto result = *this;
    result += offset;
    return result;
  }

  template<typename T>
  TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<T>>>
      TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<
      T>>>::operator -(std::ptrdiff_t offset) const {
    auto result = *this;
    result -= offset;
    return result;
  }

  template<typename T>
  std::ptrdiff_t TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<
      T>>>::operator -(const TrialIterator& other) const {
    return m_offset - other.m_offset;
  }

  template<typename T>
  bool TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<
      T>>>::operator ==(const TrialIterator& other) const {
    return m_offset == other.m_offset;
  }

  template<typename T>
  bool TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<
      T>>>::operator !=(const TrialIterator& other) const {
    return m_offset != other.m_offset;
  }

  template<typename T>
  bool TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<
      T>>>::operator <(const TrialIterator& other) const {
    return m_offset < other.m_offset;
  }

  template<typename T>
  bool TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<
      T>>>::operator <=(const TrialIterator& other) const {
    return m_offset <= other.m_offset;
  }

  template<typename T>
  bool TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<
      T>>>::operator >(const TrialIterator& other) const {
    return m_offset > other.m_offset;
  }

  template<typename T>
  bool TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<
      T>>>::operator >=(const TrialIterator& other) const {
    return m_offset >= other.m_offset;
  }

  template<typename T>
  typename const TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<
      T>>>::Sample& TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<
      T>>>::operator *() {
    return (*this)[0];
  }

  template<typename T>
  typename const TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<
      T>>>::Sample* TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<
      T>>>::operator ->() {
    m_sample.emplace((*m_trial)[m_offset]);
    return std::addressof((*this)[0]);
  }

  template<typename T>
  typename const TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<
      T>>>::Sample& TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<
      T>>>::operator [](std::ptrdiff_t offset) {
    m_sample = (*m_trial)[m_offset + offset];
    return m_sample;
  }

  template<typename T>
  TrialIterator<T, std::enable_if_t<returns_sample_by_copy_v<
      T>>>::TrialIterator(const Trial& trial, std::size_t offset)
    : m_trial(&trial),
      m_offset(offset) {}

  template<typename T>
  TrialIterator<T, std::enable_if_t<returns_sample_by_reference_v<T>>>&
      TrialIterator<T, std::enable_if_t<returns_sample_by_reference_v<
      T>>>::operator ++() {
    ++m_offset;
    return *this;
  }

  template<typename T>
  TrialIterator<T, std::enable_if_t<returns_sample_by_reference_v<T>>>
      TrialIterator<T, std::enable_if_t<returns_sample_by_reference_v<
      T>>>::operator ++(int) {
    auto copy = *this;
    ++m_offset;
    return copy;
  }

  template<typename T>
  TrialIterator<T, std::enable_if_t<returns_sample_by_reference_v<T>>>&
      TrialIterator<T, std::enable_if_t<returns_sample_by_reference_v<
      T>>>::operator --() {
    --m_offset;
    return *this;
  }

  template<typename T>
  TrialIterator<T, std::enable_if_t<returns_sample_by_reference_v<T>>>
      TrialIterator<T, std::enable_if_t<returns_sample_by_reference_v<
      T>>>::operator --(int) {
    auto copy = *this;
    --m_offset;
    return copy;
  }

  template<typename T>
  TrialIterator<T, std::enable_if_t<returns_sample_by_reference_v<T>>>&
      TrialIterator<T, std::enable_if_t<returns_sample_by_reference_v<
      T>>>::operator +=(std::ptrdiff_t offset) {
    m_offset += offset;
    return *this;
  }

  template<typename T>
  TrialIterator<T, std::enable_if_t<returns_sample_by_reference_v<T>>>&
      TrialIterator<T, std::enable_if_t<returns_sample_by_reference_v<
      T>>>::operator -=(std::ptrdiff_t offset) {
    m_offset -= offset;
    return *this;
  }

  template<typename T>
  TrialIterator<T, std::enable_if_t<returns_sample_by_reference_v<T>>>
      TrialIterator<T, std::enable_if_t<returns_sample_by_reference_v<
      T>>>::operator +(std::ptrdiff_t offset) const {
    auto result = *this;
    result += offset;
    return result;
  }

  template<typename T>
  TrialIterator<T, std::enable_if_t<returns_sample_by_reference_v<T>>>
      TrialIterator<T, std::enable_if_t<returns_sample_by_reference_v<
      T>>>::operator -(std::ptrdiff_t offset) const {
    auto result = *this;
    result -= offset;
    return result;
  }

  template<typename T>
  std::ptrdiff_t TrialIterator<T, std::enable_if_t<
      returns_sample_by_reference_v<T>>>::operator -(TrialIterator other)
      const {
    return m_offset - other.m_offset;
  }

  template<typename T>
  bool TrialIterator<T, std::enable_if_t<returns_sample_by_reference_v<
      T>>>::operator ==(TrialIterator other) const {
    return m_offset == other.m_offset;
  }

  template<typename T>
  bool TrialIterator<T, std::enable_if_t<returns_sample_by_reference_v<
      T>>>::operator !=(TrialIterator other) const {
    return m_offset != other.m_offset;
  }

  template<typename T>
  bool TrialIterator<T, std::enable_if_t<returns_sample_by_reference_v<
      T>>>::operator <(TrialIterator other) const {
    return m_offset < other.m_offset;
  }

  template<typename T>
  bool TrialIterator<T, std::enable_if_t<returns_sample_by_reference_v<
      T>>>::operator <=(TrialIterator other) const {
    return m_offset <= other.m_offset;
  }

  template<typename T>
  bool TrialIterator<T, std::enable_if_t<returns_sample_by_reference_v<
      T>>>::operator >(TrialIterator other) const {
    return m_offset > other.m_offset;
  }

  template<typename T>
  bool TrialIterator<T, std::enable_if_t<returns_sample_by_reference_v<
      T>>>::operator >=(TrialIterator other) const {
    return m_offset >= other.m_offset;
  }

  template<typename T>
  typename const TrialIterator<T, std::enable_if_t<
      returns_sample_by_reference_v<T>>>::Sample& TrialIterator<T,
      std::enable_if_t<returns_sample_by_reference_v<T>>>::operator *() const {
    return (*m_trial)[m_offset];
  }

  template<typename T>
  typename const TrialIterator<T, std::enable_if_t<
      returns_sample_by_reference_v<T>>>::Sample* TrialIterator<T,
      std::enable_if_t<returns_sample_by_reference_v<T>>>::operator ->()
      const {
    return std::addressof((*m_trial)[m_offset]);
  }

  template<typename T>
  typename const TrialIterator<T, std::enable_if_t<
      returns_sample_by_reference_v<T>>>::Sample& TrialIterator<T,
      std::enable_if_t<returns_sample_by_reference_v<T>>>::operator [](
      std::ptrdiff_t offset) const {
    return (*m_trial)[m_offset + offset];
  }

  template<typename T>
  TrialIterator<T, std::enable_if_t<returns_sample_by_reference_v<
      T>>>::TrialIterator(const Trial& trial, std::size_t offset)
    : m_trial(&trial),
      m_offset(offset) {}
}

#endif
