#ifndef ROVER_FACTOR_HPP
#define ROVER_FACTOR_HPP
#include <map>
#include <optional>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace Rover {

  //! Maps categories of a categorical variable to different dimensions.
  /*!
    \tparam T The type of the categorical variable with no operator < defined
              or std::hash specialized.
  */
  template<typename T, typename = void>
  class Factor {
    public:

      //! The type of the categorical variable.
      using Type = T;

      //! Registers a new category, if not already added.
      /*!
        \param category The category to add.
        \details Amortized O(1) complexity.
      */
      void add_category(const Type& category);

      //! Returns a unique dimension from 0 to (number of categories - 1) for
      //! a given category. Returns nullopt if the category is not registered.
      /*!
        \param category The category.
        \details O(N) complexity.
      */
      std::optional<int> find_dimension(const Type& category) const;

    private:
      std::vector<Type> m_categories;
  };

namespace Details{
  template<typename T, typename = void>
  struct is_hashable : std::false_type {};

  template<typename T>
  struct is_hashable<T, std::enable_if_t<std::is_same_v<std::invoke_result_t<
    std::hash<T>, T>, std::size_t>>> : std::true_type {};

  template<typename T>
  inline constexpr bool is_hashable_v = is_hashable<T>::value;

  template<typename T, typename = void>
  struct is_comparable : std::false_type {};

  template<typename T>
  struct is_comparable<T, std::enable_if_t<std::is_convertible_v<std::decay_t<
    decltype(std::declval<T>() < std::declval<T>())>, bool>>> :
    std::true_type {};

  template<typename T>
  inline constexpr bool is_comparable_v = is_comparable<T>::value;
}

  //! Maps categories of a categorical variable to different dimensions.
  /*!
    \tparam T The type of the categorical variable with std::hash specialized.
  */
  template<typename T>
  class Factor<T, std::enable_if_t<Details::is_hashable_v<T>>> {
    public:

      //! The type of the categorical variable.
      using Type = T;

      //! Registers a new category, if not already added.
      /*!
        \param category The category to add.
        \details Amortized O(1) complexity.
      */
      void add_category(const Type& category);

      //! Returns a unique dimension from 0 to (number of categories - 1) for
      //! a given category. Returns nullopt if the category is not registered.
      /*!
        \param category The category.
        \details Amortized O(1) complexity.
      */
      std::optional<int> find_dimension(const Type& category) const;

    private:
      std::unordered_map<Type, int> m_map;
  };

  //! Maps categories of a categorical variable to different dimensions.
  /*!
    \tparam T The type of the categorical variable with no std::hash
              specialized but operator < defined.
  */
  template<typename T>
  class Factor<T, std::enable_if_t<!Details::is_hashable_v<T> &&
      Details::is_comparable_v<T>>> {
    public:

      //! The type of the categorical variable.
      using Type = T;

      //! Registers a new category, if not already added.
      /*!
        \param category The category to add.
        \details O(log N) complexity.
      */
      void add_category(const Type& category);
      
      //! Returns a unique dimension from 0 to (number of categories - 1) for
      //! a given category. Returns nullopt if the category is not registered.
      /*!
        \param category The category.
        \details O(log N) complexity. 
      */
      std::optional<int> find_dimension(const Type& category) const;

    private:
      std::map<Type, int> m_map;
  };

  template<typename T, typename C>
  void Factor<T, C>::add_category(const Type& category) {
    if(std::find(m_categories.begin(), m_categories.end(), category) ==
        m_categories.end()) {
      m_categories.push_back(category);
    }
  }

  template<typename T, typename C>
  std::optional<int> Factor<T, C>::find_dimension(const Type& category)
      const {
    auto it = std::find(m_categories.begin(), m_categories.end(), category);
    if(it == m_categories.end()) {
      return std::nullopt;
    } else {
      return it - m_categories.begin();
    }
  }

  template<typename T>
  void Factor<T, std::enable_if_t<Details::is_hashable_v<T>>>::add_category(
      const Type& category) {
    m_map.try_emplace(category, m_map.size());
  }

  template<typename T>
  std::optional<int> Factor<T, std::enable_if_t<Details::is_hashable_v<
      T>>>::find_dimension(const Type& category) const {
    auto it = m_map.find(category);
    if(it == m_map.end()) {
      return std::nullopt;
    } else {
      return it->second;
    }
  }

  template<typename T>
  void Factor<T, std::enable_if_t<!Details::is_hashable_v<T> &&
      Details::is_comparable_v<T>>>::add_category(
      const Type& category) {
    auto it = m_map.lower_bound(category);
    if(it == m_map.end() || it->first < category || category < it->first) {
      m_map.emplace_hint(it, category, m_map.size());
    }
  }

  template<typename T>
  std::optional<int> Factor<T, std::enable_if_t<
      !Details::is_hashable_v<T> && Details::is_comparable_v<
      T>>>::find_dimension(const Type& category) const {
    auto it = m_map.find(category);
    if(it == m_map.end()) {
      return std::nullopt;
    } else {
      return it->second;
    }
  }
}

#endif
