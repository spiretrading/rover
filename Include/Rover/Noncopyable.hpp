#ifndef ROVER_NONCOPYABLE_HPP
#define ROVER_NONCOPYABLE_HPP

namespace Rover {

  struct Noncopyable {
    Noncopyable() = default;
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable& operator=(const Noncopyable&) = delete;
    Noncopyable(Noncopyable&&) = default;
    Noncopyable& operator=(Noncopyable&&) = default;
  };
}

#endif
