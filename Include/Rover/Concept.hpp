#ifndef CONCEPT_HPP
#define CONCEPT_HPP

#include <type_traits>

namespace Rover {

  /*! \struct Concept
    \brief Contains requirements and properties about a template substitution.
 */
  template<typename T>
  struct Concept {};

  /*! \struct ImplementsConcept
      \brief Specifies whether a type implements a Concept.
      \tparam InstanceType The type declaring its implementation.
      \tparam ConceptType The type of Concept implemented.
   */
  template<typename InstanceType, typename ConceptType>
  struct ImplementsConcept : std::false_type {};

  template<typename InstanceType, typename ConceptType>
  inline constexpr bool implements_concept_v =
    typename ImplementsConcept<InstanceType, ConceptType>::value;

}

#endif