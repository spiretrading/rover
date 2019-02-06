#ifndef ROVER_REFERENCE_HPP
#define ROVER_REFERENCE_HPP
#include <type_traits>
#include "Definitions.hpp"
#include "Generator.hpp"
#include "SessionVisitor.hpp"
#include "Utilities.hpp"

namespace Rover {

  //! Used to reference an existing generator, evaluating to whatever argument
  //! its referrent has generated.
  /*!
    \tparam T The type of generator to reference.
  */
  template<typename T>
  class Reference {
    static_assert(ImplementsConcept<T, Generator>::value,
      "Attempt to create a Reference to a non-Generator type");

    public:

      //! The type of generator to reference.
      using GeneratorType = T;

      using Type = typename GeneratorType::Type;

      template<typename... ArgFwd, std::enable_if_t<
          !std::is_same_v<typelist<std::decay_t<ArgFwd>...>,
          typelist<Reference>> && 
          !std::is_same_v<typelist<std::decay_t<ArgFwd>...>,
          typelist<GeneratorType>>
          >* = nullptr>
      Reference(ArgFwd&&... args);

      template<std::enable_if_t<!is_reference_v<GeneratorType>>* = nullptr>
      Reference(GeneratorType&& generator);

      template<typename SessionType>
      Type generate(SessionType& s) const;

      template<typename SessionType>
      Type generate_impl(SessionType& s) const;

      template<typename OtherType, std::enable_if_t<
          !std::is_same_v<GeneratorType, OtherType>>* = nullptr>
      bool constexpr is_same(const Reference<OtherType>& other) const;

      bool is_same(const Reference<GeneratorType>& other) const;

      auto build_session() const;

    private:
      std::shared_ptr<GeneratorType> m_generator;
  };

  template<typename U>
  Reference(U&&) -> Reference<std::decay_t<U>>;

  template<typename GeneratorType>
  template<typename... ArgFwd, std::enable_if_t<
      !std::is_same_v<typelist<std::decay_t<ArgFwd>...>,
      typelist<Reference<GeneratorType>>> &&
      !std::is_same_v<typelist<std::decay_t<ArgFwd>...>,
      typelist<GeneratorType>>>*>
  Reference<GeneratorType>::Reference(ArgFwd&&... args)
      : m_generator(new GeneratorType(std::forward<ArgFwd>(args)...)) {}

  template<typename GeneratorType>
  template<std::enable_if_t<!is_reference_v<GeneratorType>>*>
  Reference<GeneratorType>::Reference(GeneratorType&& generator)
      : m_generator(new GeneratorType(std::move(generator))) {}

  template<typename GeneratorType>
  template<typename SessionType>
  typename Reference<GeneratorType>::Type
      Reference<GeneratorType>::generate(SessionType& s) const {
    return SessionVisitor<SessionType>()(s, *this);
  }

  template<typename GeneratorType>
  template<typename SessionType>
  typename Reference<GeneratorType>::Type
      Reference<GeneratorType>::generate_impl(SessionType& s) const {
    return m_generator->generate(s);
  }

  template<typename GeneratorType>
  auto Reference<GeneratorType>::build_session() const {
    return m_generator->build_session();
  }

  template<typename GeneratorType>
  template<typename OtherType, std::enable_if_t<
      !std::is_same_v<GeneratorType, OtherType>>*>
  bool constexpr Reference<GeneratorType>::is_same(
    const Reference<OtherType>& other) const {
    return false;
  }

  template<typename GeneratorType>
  bool Reference<GeneratorType>::is_same(
    const Reference<GeneratorType>& other) const {
    return m_generator.get() == other.m_generator.get();
  }

  template<typename Type>
  struct ensure_reference {
    using type = std::conditional_t<is_reference_v<Type>, Type,
      Reference<Type>>;
  };

  template<typename Type>
  using ensure_reference_t = typename ensure_reference<Type>::type;

  template<template<typename...> class Impl, typename... ArgsFwd>
  auto make_reference(ArgsFwd&&... args) {
    return Reference<Impl<std::decay_t<ArgsFwd>...>>(
      std::forward<ArgsFwd>(args)...);
  }

  template<typename Type>
  struct ImplementsConcept<Reference<Type>, Generator> : std::true_type {};
}

#endif
