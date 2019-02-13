#ifndef ROVER_NONCOPYABLE_HPP
#define ROVER_NONCOPYABLE_HPP

namespace Rover {
namespace {

  /** Base class that disables default copy construction and assignment. */
  class Noncopyable {
    protected:
      constexpr Noncopyable() = default;
      Noncopyable(const Noncopyable&) = delete;
      Noncopyable& operator =(const Noncopyable&) = delete;
  };
}
}

#endif
