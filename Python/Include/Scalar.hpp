#ifndef ROVER_PYTHON_SCALAR_HPP
#define ROVER_PYTHON_SCALAR_HPP
#include <type_traits>
#include <dlib/algs.h>
#include <pybind11/pybind11.h>

namespace Rover {

  //! PythonScalar forward declaration.
  template<typename T>
  class PythonScalar;

  //! Pointer type for PythonScalar convertible to both T* and
  //! PythonScalar<T>*.
  /*
    \tparam T The type of the scalar.
  */
  template<typename T>
  class PythonScalarPointer {
    public:

      //! Constructs the object using a raw PythonScalar<T> pointer.
      PythonScalarPointer(PythonScalar<T>* ptr);

      //! Converts the pointer to PythonScalar<T>*.
      operator PythonScalar<T>*() const;

      //! Converts the pointer to T*.
      operator T*() const;

    private:
      PythonScalar<T>* m_ptr;
  };

  //! Constant pointer type for PythonScalar convertible to both const T* and
  //! const PythonScalar<T>*.
  /*
    \tparam T The type of the scalar.
  */
  template<typename T>
  class PythonScalarConstPointer {
    public:

      //! Constructs the object using a raw PythonScalar<T> constant pointer.
      PythonScalarConstPointer(const PythonScalar<T>* ptr);

      //! Converts the pointer to const PythonScalar<T>*.
      operator const PythonScalar<T>*() const;

      //! Converts the pointer to const T*.
      operator const T*() const;

    private:
      const PythonScalar<T>* m_ptr;
  };

  //! Adapts a scalar type to pybind11::object by providing the one-way
  //! conversion from Python objects and forwarding scalar semantics.
  /*
    \tparam T The type of the scalar - an arithmetic type.
  */
  template<typename T>
  class PythonScalar {
    public:

      //! Constructs an uninitialized PythonScalar.
      PythonScalar() = default;

      //! Constructs a PythonScalar from a Python object.
      PythonScalar(const pybind11::object& obj);

      //! Constructs a PythonScalar from an instance of the scalar.
      PythonScalar(T value);

      //! Converts the PythonScalar to the type of the scalar.
      operator T() const;

      //! Converts the PythonScalar to a reference to the scalar.
      operator T&();

      //! Produces a PythonScalarPointer corresponding to the PythonScalar.
      PythonScalarPointer<T> operator &();

      //! Produces a PythonScalarConstPointer corresponding to the
      //! PythonScalar.
      PythonScalarConstPointer<T> operator &() const;

    private:
      friend PythonScalarPointer<T>;
      friend PythonScalarConstPointer<T>;
      template<typename T> friend PythonScalar<T> abs(PythonScalar<T>);

      T m_value;
  };

  template<typename T>
  PythonScalarPointer<T>::PythonScalarPointer(PythonScalar<T>* ptr) 
    : m_ptr(ptr) {}

  template<typename T>
  PythonScalarPointer<T>::operator PythonScalar<T>*() const {
    return m_ptr;
  }

  template<typename T>
  PythonScalarPointer<T>::operator T*() const {
    return &(m_ptr->m_value);
  }

  template<typename T>
  PythonScalarConstPointer<T>::PythonScalarConstPointer(const PythonScalar<T>*
      ptr)
    : m_ptr(ptr) {}

  template<typename T>
  PythonScalarConstPointer<T>::operator const PythonScalar<T>*() const {
    return m_ptr;
  }

  template<typename T>
  PythonScalarConstPointer<T>::operator const T*() const {
    return &(m_ptr->m_value);
  }

  template<typename T>
  PythonScalar<T>::PythonScalar(const pybind11::object& obj)
    : m_value(obj.template cast<T>()) {}

  template<typename T>
  PythonScalar<T>::PythonScalar(T value)
    : m_value(std::move(value)) {}

  template<typename T>
  PythonScalarPointer<T> PythonScalar<T>::operator &() {
    return this;
  }

  template<typename T>
  PythonScalarConstPointer<T> PythonScalar<T>::operator &() const {
    return this;
  }

  template<typename T>
  PythonScalar<T>::operator T() const {
    return m_value;
  }

  template<typename T>
  PythonScalar<T>::operator T&() {
    return m_value;
  }

  template<typename T>
  PythonScalar<T> abs(PythonScalar<T> scalar) {
    ::abs(scalar.m_value);
    return std::move(scalar);
  }
}

namespace pybind11 {
  template<typename T>
  object cast(const Rover::PythonScalar<T>& scalar) {
    return cast(static_cast<T>(scalar));
  }
}

namespace dlib {
  template<typename T>
  class is_same_type<T, Rover::PythonScalar<T>> : private std::true_type {};

  template<typename T>
  class is_same_type<Rover::PythonScalar<T>, T> : private std::true_type {};
}

#endif
