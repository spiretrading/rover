#ifndef COMPOSITE_HPP
#define COMPOSITE_HPP
#include <type_traits>
#include "Concept.hpp"
#include "Generator.hpp"
#include "Reference.hpp"
#include "Session.hpp"
#include "Utilities.hpp"

namespace Rover {

  /*! \class Composite
      \brief Evaluates a set of generators while maintaining their dependencies.
      \tparam References Types of generator references
  */
  template<typename... References>
  class Composite {
    static_assert(std::conjunction_v<is_reference<References>...>,
      "One or more arguments of Composite is not a Generator");

  public:

    using Type = std::tuple<typename References::Type...>;

    /** Constructs a Composite from generators. */
    template<typename... ReferencesFwd, std::enable_if_t<
      !std::is_same_v<typelist<std::decay_t<ReferencesFwd>...>,
      typelist<Composite>>>* = nullptr>
    Composite(ReferencesFwd&&... params);

    Composite(const Composite&) = delete;

    template<typename Session>
    constexpr Type generate(Session& session) const;

    auto build_session() const;

  private:
    std::tuple<References...> m_references;
  };

  template<typename... ReferencesFwd>
  Composite(ReferencesFwd&&...) ->
    Composite<ensure_reference_t<std::decay_t<ReferencesFwd>>...>;

  template<typename... References>
  template<typename... ReferencesFwd, std::enable_if_t<
    !std::is_same_v<typelist<std::decay_t<ReferencesFwd>...>,
    typelist<Composite<References...>>>>*>
  Composite<References...>::Composite(ReferencesFwd&&... params)
    : m_references(std::forward<ReferencesFwd>(params)...) {};

  template<typename... References>
  template<typename Session>
  typename constexpr Composite<References...>::Type 
  Composite<References...>::generate(Session& s) const {
    return std::apply([&s](auto&&... references) {
      return std::make_tuple(references.generate(s)...);
    }, m_references);
  }

  template<typename... References>
  auto Composite<References...>::build_session() const {
    auto dependencies = std::apply([](auto&&... references) {
      return std::tuple_cat(references.build_session()...);
    }, m_references);
    auto references = std::tuple_cat(m_references, dependencies);

    return std::apply([](auto&&... references) {
      return std::make_tuple(Session::Entry(references)...);
    }, references);
  }

  template<typename... References>
  struct ImplementsConcept<Composite<References...>, Generator> :
    std::true_type {};

}

#endif
