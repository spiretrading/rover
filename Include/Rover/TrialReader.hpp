#ifndef ROVER_TRIAL_READER_HPP
#define ROVER_TRIAL_READER_HPP
#include <functional>
#include <vector>

namespace Rover {

  template<typename T>
  class TrialReader {
    public:

      using Type = T;

      struct Sample {

        using Result = Type;

        using Arguments = std::vector<Type>;

        Result m_result;

        Arguments m_arguments;
      };

      template<typename GetFwd>
      TrialReader(GetFwd&& get, std::size_t size);

      Sample operator [](std::size_t i) const;

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
