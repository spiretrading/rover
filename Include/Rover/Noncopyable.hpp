#ifndef ROVER_NONCOPYABLE_HPP
#define ROVER_NONCOPYABLE_HPP

namespace Rover {

  /*! \class Noncopyable
      \brief Represents a non-copyable object that can be moved.
      \detail User classes should inherit from Noncopyable to disable copy
              operations. Private inheritance is enough.
  */
  class Noncopyable {
  protected:

    //! Empty default constructor.
    Noncopyable() = default;

    //! Empty default destructor.
    /*
      \detail Explicit definition is unnecessary but kept for rule of 5 
              consistency
    */
    ~Noncopyable() = default;

    //! Deleted copy constructor
    Noncopyable(const Noncopyable&) = delete;

    //! Deleted copy assignment operator
    Noncopyable& operator=(const Noncopyable&) = delete;

    //! Empty default move constructor
    Noncopyable(Noncopyable&&) = default;

    //! Empty default move assignment operator
    Noncopyable& operator=(Noncopyable&&) = default;
  };
}

#endif
