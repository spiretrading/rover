#ifndef ROVER_SELECT_HPP
#define ROVER_SELECT_HPP
#include <type_traits>
#include <vector>
#include "Rover/Autobox.hpp"
#include "Rover/Evaluator.hpp"
#include "Rover/Range.hpp"

namespace Rover {
namespace Details {
  struct AnyIndex {
    template<typename T>
    operator const T() const;
  };

  template<typename T, typename = std::void_t<>>
  struct has_subscript : std::false_type {};

  template<typename T>
  struct has_subscript<T, std::void_t<decltype(std::declval<T>()[
    std::declval<AnyIndex>()])>> : std::true_type {};

  template<typename T>
  inline constexpr bool has_subscript_v = has_subscript<T>::value;

  template<typename T, typename = std::void_t<>>
  struct is_list : std::false_type {};

  template<typename T>
  struct is_list<T, std::enable_if_t<std::is_same_v<std::decay_t<decltype(
    *std::begin(std::declval<T>()))>, std::decay_t<decltype(
    std::declval<T>()[0])>>>> : std::true_type {};

  template<typename T>
  inline constexpr bool is_list_v = is_list<T>::value;

  template<typename T, typename = std::void_t<>>
  struct is_array : std::false_type {};

  template<typename T>
  struct is_array<T, std::enable_if_t<has_subscript_v<T> && is_list_v<T>>> :
    std::true_type {};

  template<typename T>
  inline constexpr bool is_array_v = is_array<T>::value;

  template<typename T, typename = std::void_t<>>
  struct is_map : std::false_type {};

  template<typename T>
  struct is_map<T, std::enable_if_t<has_subscript_v<T> && !is_list_v<T>>> :
    std::true_type {};

  template<typename T>
  inline constexpr bool is_map_v = is_map<T>::value;

  template<typename T, typename = std::void_t<>>
  struct is_set : std::false_type {};

  template<typename T>
  struct is_set<T, std::enable_if_t<!has_subscript_v<T>>> : std::true_type {};

  template<typename T>
  inline constexpr bool is_set_v = is_set<T>::value;
}

  //! Evaluates the generator stored at the selected index in the array
  //! representation of a container.
  /*!
    \tparam C The type of the container.
    \tparam S Generator evaluating to an index in the array.
  */ 
  template<typename C, typename S, typename = std::void_t<>>
  class Select;

  //! Evaluates the generator stored at the selected index in an array.
  /*!
    \tparam C Array-like container defining value_type, begin, and integer
              indexing.
    \tparam S Generator evaluating to an index in the array.
  */
  template<typename C, typename S>
  class Select<C, S, std::enable_if_t<Details::is_array_v<C>>> {
    public:
      
      //! Array-like container defining value_type, begin, and integer indexing.
      using Container = C;

      //! Generator evaluating to an index in the array.
      using Selector = S;

      //! The evaluated type.
      using Type = typename autobox_t<typename Container::value_type>::Type;

      //! Creates a Select with a selector evaluating randomly to any index in
      //! Container.
      /*!
        \param container Array-like container defining value_type, begin,
                         and integer indexing.
      */
      explicit Select(Container container);

      //! Creates a Select with a user-defined selector.
      /*!
        \param container Array-like container defining value_type, begin,
                         and integer indexing.
        \param selector Generator evaluating to an index in the array.
      */
      Select(Container container, Selector selector);

      //! Evaluates the generator.
      /*!
        \param evaluator The evaluator keeping track of the current session.
      */
      Type generate(Evaluator& evaluator);

    private:
      Container m_container;
      Selector m_selector;
  };

  //! Evaluates the generator stored at the selected index in the array
  //! representation of a set.
  /*!
    \tparam C Set-like container defining value_type, begin, and end.
    \tparam S Generator evaluating to an index in the array.
  */
  template<typename C, typename S>
  class Select<C, S, std::enable_if_t<Details::is_set_v<C>>> {
    public:
      
      //! Set-like container defining value_type, begin, and end.
      using Container = C;

      //! Generator type of set keys.
      using Element = autobox_t<typename Container::value_type>;

      //! Generator evaluating to an index in the array.
      using Selector = S;

      //! The evaluated type.
      using Type = typename Element::Type;

      //! Creates a Select with a selector evaluating randomly to any index in
      //! Container.
      /*!
        \param container Set-like container defining value_type, begin, and end.
      */
      explicit Select(const Container& container);

      //! Creates a Select with a user-defined selector.
      /*!
        \param container Set-like container defining value_type, begin, and end.
        \param selector Generator evaluating to an index in the array.
      */
      Select(const Container& container, Selector selector);

      //! Evaluates the generator.
      /*!
        \param evaluator The evaluator keeping track of the current session.
      */
      Type generate(Evaluator& evaluator);

    private:
      std::vector<Element> m_container;
      Selector m_selector;
  };

  //! Evaluates the generator stored at the selected index in the array
  //! representation of map values.
  /*!
    \tparam C Map-like container defining mapped_type, begin, and end.
    \tparam S Generator evaluating to an index in the array.
  */
  template<typename C, typename S>
  class Select<C, S, std::enable_if_t<Details::is_map_v<C>>> {
    public:
      
      //! Map-like container defining mapped_type, begin, and end.
      using Container = C;

      //! Generator type of map values.
      using Element = autobox_t<typename Container::mapped_type>;

      //! Generator evaluating to an index in the array.
      using Selector = S;

      //! The evaluated type.
      using Type = typename Element::Type;

      //! Creates a Select with a selector evaluating randomly to any index in
      //! Container.
      /*!
        \param container Map-like container defining mapped_type, begin, and end.
      */
      explicit Select(const Container& container);

      //! Creates a Select with a user-defined selector.
      /*!
        \param container Map-like container defining mapped_type, begin, and end.
        \param selector Generator evaluating to an index in the array.
      */
      Select(const Container& container, Selector selector);

      //! Evaluates the generator.
      /*!
        \param evaluator The evaluator keeping track of the current session.
      */
      Type generate(Evaluator& evaluator);

    private:
      std::vector<Element> m_container;
      Selector m_selector;
  };

  template<typename C>
  Select(C) -> Select<C, Range<int, int>>;

  template<typename C, typename S>
  Select(C, S) -> Select<C, S>;

  template<typename C, typename S>
  Select<C, S, std::enable_if_t<Details::is_array_v<C>>>::Select(
      Container container)
    : m_container(std::move(container)),
      m_selector(Range(0, static_cast<int>(m_container.size()) - 1)) {}

  template<typename C, typename S>
  Select<C, S, std::enable_if_t<Details::is_array_v<C>>>::Select(
      Container container, Selector selector)
    : m_container(std::move(container)),
      m_selector(std::move(selector)) {}

  template<typename C, typename S>
  typename Select<C, S, std::enable_if_t<Details::is_array_v<C>>>::Type
      Select<C, S, std::enable_if_t<Details::is_array_v<C>>>::generate(
      Evaluator& evaluator) {
    auto key = evaluator.evaluate(m_selector);
    auto& generator = m_container[key];
    auto value = evaluator.evaluate(make_autobox(generator));
    return value;
  }

  template<typename C, typename S>
  Select<C, S, std::enable_if_t<Details::is_set_v<C>>>::Select(
      const Container& container)
      : m_selector(Range(0, static_cast<int>(container.size()) - 1)) {
    for(auto generator : container) {
      m_container.push_back(std::move(generator));
    }
  }

  template<typename C, typename S>
  Select<C, S, std::enable_if_t<Details::is_set_v<C>>>::Select(
      const Container& container, Selector selector)
      : m_selector(std::move(selector)) {
    for(auto generator : container) {
      m_container.push_back(std::move(generator));
    }
  }

  template<typename C, typename S>
  typename Select<C, S, std::enable_if_t<Details::is_set_v<C>>>::Type
      Select<C, S, std::enable_if_t<Details::is_set_v<C>>>::generate(
      Evaluator& evaluator) {
    auto key = evaluator.evaluate(m_selector);
    auto& generator = m_container[key];
    auto value = evaluator.evaluate(generator);
    return value;
  }

  template<typename C, typename S>
  Select<C, S, std::enable_if_t<Details::is_map_v<C>>>::Select(
      const Container& container)
      : m_selector(Range(0, static_cast<int>(container.size()) - 1)) {
    for(auto& p : container) {
      m_container.push_back(p.second);
    }
  }

  template<typename C, typename S>
  Select<C, S, std::enable_if_t<Details::is_map_v<C>>>::Select(
      const Container& container, Selector selector)
      : m_selector(std::move(selector)) {
    for(auto& p : container) {
      m_container.push_back(p.second);
    }
  }

  template<typename C, typename S>
  typename Select<C, S, std::enable_if_t<Details::is_map_v<C>>>::Type
      Select<C, S, std::enable_if_t<Details::is_map_v<C>>>::generate(
      Evaluator& evaluator) {
    auto key = evaluator.evaluate(m_selector);
    auto& generator = m_container[key];
    auto value = evaluator.evaluate(generator);
    return value;
  }
}

#endif
