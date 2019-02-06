#ifndef ROVER_ARITHMETICS_HPP
#define ROVER_ARITHMETICS_HPP
#include <cmath>
#include <type_traits>

namespace Rover {

  template<typename T, typename = void>
  class NextValue;

  template<typename T>
  class NextValue<T, std::enable_if_t<std::is_integral_v<T>>> {

    public:
    
      template<typename Fwd>
      constexpr NextValue(Fwd&& val)
          : m_val(std::forward<Fwd>(val) + 1) {}
    
      constexpr operator T() const {
        return m_val;
      }
    
    private:
      T m_val;
  };

  template<typename T>
  struct NextValue<T, std::enable_if_t<std::is_floating_point_v<T>>> {
    
    public:
    
      template<typename Fwd>
      constexpr NextValue(Fwd&& val)
          : m_val(std::nextafter(std::forward<Fwd>(val),
              std::numeric_limits<T>::max())) {}
    
      constexpr operator T() const {
        return m_val;
      }
    
    private:
      T m_val;
  };

  template<typename Fwd>
  NextValue(Fwd&&) -> NextValue<std::decay_t<Fwd>>;

  template<typename T, typename = void>
  class PrevValue;

  template<typename T>
  class PrevValue<T, std::enable_if_t<std::is_integral_v<T>>> {

    public:
    
      template<typename Fwd>
      constexpr PrevValue(Fwd&& val)
          : m_val(std::forward<Fwd>(val) - 1) {}
    
      constexpr operator T() const {
        return m_val;
      }
    
    private:
      T m_val;
  };

  template<typename T>
  struct PrevValue<T, std::enable_if_t<std::is_floating_point_v<T>>> {
    
    public:
    
      template<typename Fwd>
      constexpr PrevValue(Fwd&& val)
          : m_val(std::nextafter(std::forward<Fwd>(val),
              std::numeric_limits<T>::lowest())) {}
    
      constexpr operator T() const {
        return m_val;
      }
    
    private:
      T m_val;
  };
  
  template<typename Fwd>
  PrevValue(Fwd&&) -> PrevValue<std::decay_t<Fwd>>;

  template<typename Begin, typename End>
  void check_range(const Begin& begin, const End& end) {
    if(end < begin) {
      throw std::out_of_range("Invalid range!");
    }
  }

}

#endif
