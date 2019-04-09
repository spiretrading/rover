#ifndef ROVER_TRIAL_READER_HPP
#define ROVER_TRIAL_READER_HPP
#include <functional>
#include <vector>

namespace Rover {

  //! Interface to pass adapted Trial Samples to typed Algorithms.
  /*!
    \tparam T CompType used by the algorithm.
  */
  template<typename T>
  class TrialReader {
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

      //! Creates a TrialReader.
      /*!
        \param get The function taking an index i and returning the ith adapted
                   sample.
        \param size The total number of samples.
      */
      template<typename GetFwd>
      TrialReader(GetFwd&& get, std::size_t size);

      //! Returns ith adapted sample.
      Sample operator [](std::size_t i) const;

      //! Returns the number of the samples.
      std::size_t size() const;

    private:
      std::function<Sample (std::size_t)> m_get;
      std::size_t m_size;
  };

  template<typename C>
  template<typename GetFwd>
  TrialReader<C>::TrialReader(GetFwd&& get, std::size_t size)
    : m_get(std::forward<GetFwd>(get)),
      m_size(size) {}

  template<typename C>
  typename TrialReader<C>::Sample TrialReader<C>::operator [](
      std::size_t i) const {
    return m_get(i);
  }

  template<typename C>
  std::size_t TrialReader<C>::size() const {
    return m_size;
  }
}

#endif
