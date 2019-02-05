#ifndef ARITHMETICS_HPP
#define ARITHMETICS_HPP

#include <cmath>
#include <type_traits>

namespace Rover {
namespace Arithmetics {

  template<typename T, typename = void>
  class Next;

  template<typename T>
  class Next<T, std::enable_if_t<std::is_integral_v<T>>> {

    public:
    
      template<typename Fwd>
      constexpr Next(Fwd&& val)
        : m_val(std::forward<Fwd>(val) + 1) {}
    
      constexpr operator T() const {
        return m_val;
      }
    
    private:
      T m_val;
  };

  template<typename T>
  struct Next<T, std::enable_if_t<std::is_floating_point_v<T>>> {
    
    public:
    
      template<typename Fwd>
      constexpr Next(Fwd&& val)
        : m_val(std::nextafter(std::forward<Fwd>(val),
            std::numeric_limits<T>::max())) {}
    
      constexpr operator T() const {
        return m_val;
      }
    
    private:
      T m_val;
  };

  template<typename Fwd>
  Next(Fwd&&) -> Next<std::decay_t<Fwd>>;

  template<typename T, typename = void>
  class Prev;

  template<typename T>
  class Prev<T, std::enable_if_t<std::is_integral_v<T>>> {

    public:
    
      template<typename Fwd>
      constexpr Prev(Fwd&& val)
        : m_val(std::forward<Fwd>(val) - 1) {}
    
      constexpr operator T() const {
        return m_val;
      }
    
    private:
      T m_val;
  };

  template<typename T>
  struct Prev<T, std::enable_if_t<std::is_floating_point_v<T>>> {
    
    public:
    
      template<typename Fwd>
      constexpr Prev(Fwd&& val)
        : m_val(std::nextafter(std::forward<Fwd>(val),
            std::numeric_limits<T>::lowest())) {}
    
      constexpr operator T() const {
        return m_val;
      }
    
    private:
      T m_val;
  };
  
  template<typename Fwd>
  Prev(Fwd&&)->Prev<std::decay_t<Fwd>>;
}
}
#endif