#ifndef ROVER_BASIS_HPP
#define ROVER_BASIS_HPP
#include <algorithm>
#include <numeric>
#include <random>
#include <tuple>
#include <variant>
#include <vector>
#include "Rover/Factor.hpp"
#include "Rover/Sample.hpp"
#include "Rover/ScalarView.hpp"
#include "Rover/VariableTraits.hpp"

namespace Rover {
namespace Details {
  template<typename T>
  struct add_factor;

  template<typename... T>
  struct add_factor<std::tuple<T...>> {
    using type = std::tuple<std::variant<T, Factor<T>>...>;
  };

  template<typename T>
  using add_factor_t = typename add_factor<T>::type;
}

  //! Arguments held by basis, allowing for both value element and factor
  //! support.
  /*!
    \tparam A The type of the arguments from the original sample.
  */
  template<typename A>
  class BasisArguments {
    public:

      //! Creates BasisArguments using original arguments.
      /*!
        \param args The original arguments.
      */
      explicit BasisArguments(const A& args);

      //! Makes the argument at a given index a value element and sets the
      //! value.
      /*!
        \param index The index of the element to set.
        \param value The value of the element.
      */
      template<typename T>
      void set_value(std::size_t index, T&& value);

      //! Adds a category to the factor at a given index. If the element at
      //! the index is not currently a factor, makes it a factor.
      /*!
        \param index The index of the element to set.
        \param category The category to add.
      */
      template<typename T>
      void add_category(std::size_t index, T&& category);

      //! Returns the number of arguments.
      std::size_t size() const;

      //! Applies a function template to every element.
      /*!
        \param func The function template.
      */
      template<typename Func>
      void visit(Func&& func);

      //! Applies a function template to every element.
      /*!
        \param func The function template.
      */
      template<typename Func>
      void visit(Func&& func) const;

    private:
      using Arguments = Details::add_factor_t<A>;

      Arguments m_arguments;
  };

  template<typename ArgumentsFwd>
  BasisArguments(ArgumentsFwd&&) -> BasisArguments<std::decay_t<ArgumentsFwd>>;

  //! Basis of variables used to produce scalar encodings for samples.
  /*!
    \tparam S The type of the sample.
    \tparam T The type of the scalar.
  */
  template<typename S, typename T>
  class Basis {
    public:

      //! The type of the sample.
      using Sample = S;

      //! The type of the result.
      using Result = typename Sample::Result;

      //! The type of the arguments.
      using Arguments = typename Sample::Arguments;

      //! The type of the scalar.
      using ScalarType = T;

      //! The scalar type of the sample.
      using ScalarSample = Rover::ScalarSample<T>;

      //! The scalar type of the result.
      using ScalarResult = typename ScalarSample::Result;

      //! The scalar type of the arguments.
      using ScalarArguments = typename ScalarSample::Arguments;

      //! Builds a basis from a trial by finding appropriate elements.
      /*!
        \tparam Trial The type of the trial.
        \param trail The trial.
      */
      template<typename Trial>
      explicit Basis(const Trial& trial);

      //! Converts arguments to the scalar type.
      /*!
        \param arguments The arguments to convert.
      */
      ScalarArguments apply(const Arguments& arguments) const;

      //! Converts a sample to the scalar type.
      /*!
        \param sample The sample to convert.
      */
      ScalarSample apply(const Sample& sample) const;

      //! Restores the original result from a scalar result.
      /*!
        \param result The scalar result.
      */
      auto restore_result(ScalarResult result) const;

    private:
      Result m_result;
      BasisArguments<Arguments> m_arguments;
      std::vector<bool> m_to_negate;

      static std::vector<std::size_t> find_categorical(const Arguments& args);
      template<typename Trial>
      void compute_factors(const Trial& trial, const std::vector<std::size_t>&
        indexes);
      ScalarResult result_cast(const Result& value) const;
  };

namespace Details {
  template<typename R, typename X, typename Y>
  constexpr std::enable_if_t<!std::is_same_v<std::decay_t<R>,
      std::decay_t<X>> || !std::is_same_v<std::decay_t<R>, std::decay_t<Y>>,
      bool> solve_basis(R&, std::vector<bool>::reference to_negate, const X&,
      const Y&) {
    return false;
  }

  template<typename R, typename X, typename Y>
  std::enable_if_t<std::is_same_v<std::decay_t<R>, std::decay_t<X>> &&
      std::is_same_v<std::decay_t<R>, std::decay_t<Y>>, bool>
      solve_basis(R& r, std::vector<bool>::reference to_negate, const X& x,
      const Y& y) {
    if(x == y) {
      return false;
    }
    if(x + y == x) {
      r = x;
      to_negate = x + y < y;
    } else {
      r = y;
      to_negate = x + y < x;
    }
    return true;
  }

  template<typename R, typename V, typename B, typename C = void>
  struct BasisApplier {
    static void apply(R&, bool, const V&, const B&) {}
  };

  template<typename R, typename V, typename B>
  struct BasisApplier<R, V, B, std::enable_if_t<continuous<R, V>::value &&
      std::is_same_v<V, B>>> {
    static void apply(R& r, bool to_negate, const V& v, const B& b) {
      if(to_negate) {
        r = -static_cast<R>(v / b);
      } else {
        r = static_cast<R>(v / b);
      }
    }
  };

  template<typename R, typename V, typename B>
  void apply_basis(R& r, bool to_negate, const V& v, const B& b) {
    BasisApplier<R, V, B>::apply(r, to_negate, v, b);
  }

  template<typename A, typename T, typename C = void>
  struct ValueUploader {
    template<typename ValueFwd>
    static void upload(A& arg, ValueFwd&& value) {}
  };

  template<typename A, typename T>
  struct ValueUploader<A, T, std::enable_if_t<std::is_same_v<decltype(
      std::declval<A>() = std::declval<T>()), A&>>> {
    template<typename ValueFwd>
    static void upload(A& arg, ValueFwd&& value) {
      arg = std::forward<ValueFwd>(value);
    }
  };

  template<typename A, typename T>
  void upload_value(A& arg, T&& value) {
    ValueUploader<A, std::decay_t<T>>::upload(arg, std::forward<T>(value));
  }

  template<typename A, typename T, typename C = void>
  struct CategoryUploader {
    template<typename CategoryFwd>
    static void upload(A& arg, CategoryFwd&& category) {}
  };

  template<typename A, typename T>
  struct CategoryUploader<A, T, std::enable_if_t<std::is_same_v<decltype(
      std::declval<A>() = std::declval<Factor<T>>()), A&>>> {
    template<typename CategoryFwd>
    static void upload(A& arg, CategoryFwd&& category) {
      auto visited = false;
      static_assert(std::is_same_v<A, std::variant<T, Factor<T>>>);
      std::visit([&](auto& factor) {
        if constexpr(std::is_same_v<std::decay_t<decltype(factor)>,
            Factor<T>>) {
          factor.add_category(std::forward<CategoryFwd>(category));
          visited = true;
        }
      }, arg);
      if(!visited) {
        auto factor = Factor<T>();
        factor.add_category(std::forward<CategoryFwd>(category));
        arg = std::move(factor);
      }
    }
  };

  template<typename A, typename T>
  void upload_category(A& arg, T&& category) {
    CategoryUploader<A, std::decay_t<T>>::upload(arg, std::forward<T>(
      category));
  }

  template<typename F, typename T>
  struct DimensionFinder {
    static std::optional<std::size_t> find(const F&, const T&) {
      return std::nullopt;
    }
  };

  template<typename T>
  struct DimensionFinder<Factor<T>, T> {
    static std::optional<std::size_t> find(const Factor<T>& factor, const T&
        category) {
      return factor.find_dimension(category);
    }
  };

  template<typename F, typename T>
  std::optional<std::size_t> find_dimension(const F& factor, const T&
      category) {
    return DimensionFinder<F, T>::find(factor, category);
  }
}

  template<typename A>
  BasisArguments<A>::BasisArguments(const A& args)
    : m_arguments(args) {}

  template<typename A>
  template<typename T>
  void BasisArguments<A>::set_value(std::size_t index, T&& value) {
    visit_arguments([&](auto& arg, auto i) {
      if(index == i) {
        Details::upload_value(arg, std::forward<T>(value));
      }
    }, m_arguments);
  }

  template<typename A>
  template<typename T>
  void BasisArguments<A>::add_category(std::size_t index, T&& category) {
    visit_arguments([&](auto& arg, auto i) {
      if(index == i) {
        Details::upload_category(arg, std::forward<T>(category));
      }
    }, m_arguments);
  }

  template<typename A>
  std::size_t BasisArguments<A>::size() const {
    return arguments_size(m_arguments);
  }

  template<typename A>
  template<typename Func>
  void BasisArguments<A>::visit(Func&& func) {
    visit_arguments(std::forward<Func>(func), m_arguments);
  }

  template<typename A>
  template<typename Func>
  void BasisArguments<A>::visit(Func&& func) const {
    visit_arguments(std::forward<Func>(func), m_arguments);
  }

  template<typename S, typename T>
  template<typename Trial>
  Basis<S, T>::Basis(const Trial& trial)
      : m_result(trial[0].m_result),
        m_arguments(trial[0].m_arguments),
        m_to_negate(1 + m_arguments.size()) {
    auto order = std::vector<std::size_t>(trial.size());
    auto generator = std::mt19937(std::random_device()());
    auto factor_indexes = find_categorical(trial[0].m_arguments);
    std::iota(order.begin(), order.end(), std::size_t(0));
    for(auto i = std::size_t(0), last_factor_index = std::size_t(0); i <
        m_arguments.size(); ++i) {
      if(last_factor_index < factor_indexes.size() &&
          factor_indexes[last_factor_index] == i) {
        ++last_factor_index;
        continue;
      }
      std::shuffle(order.begin(), order.end(), generator);
      const auto& first_arguments = trial[order[0]].m_arguments;
      visit_arguments([&](const auto& lhs, auto j) {
        if(i == j) {
          auto to_break = false;
          for(auto l = std::size_t(1); l < order.size() && !to_break; ++l) {
            const auto& second_arguments = trial[order[l]].m_arguments;
            visit_arguments([&](const auto& rhs, auto k) {
              auto val = lhs;
              if(j == k && Details::solve_basis(val, m_to_negate[i + 1],
                  lhs, rhs)) {
                m_arguments.set_value(i, std::move(val));
                to_break = true;
              }
            }, second_arguments);
          }
        }
      }, first_arguments);
    }
    std::shuffle(order.begin(), order.end(), std::move(generator));
    const auto& first_result = trial[order[0]].m_result;
    for(auto l = std::size_t(1); l < order.size(); ++l) {
      const auto& second_result = trial[order[l]].m_result;
      if(first_result != second_result) {
        Details::solve_basis(m_result, m_to_negate[0], first_result,
          second_result);
      }
    }
    if(!factor_indexes.empty()) {
      compute_factors(trial, factor_indexes);
    }
  }
  
  template<typename S, typename T>
  typename Basis<S, T>::ScalarArguments Basis<S, T>::apply(const Arguments&
      arguments) const {
    auto result = ScalarArguments();
    visit_arguments([&](const auto& arg, auto i) {
      m_arguments.visit([&](const auto& identity, auto j) {
        if(i == j) {
          std::visit([&](const auto& val) {
            if constexpr(is_factor_v<std::decay_t<decltype(val)>>) {
              auto dimension = Details::find_dimension(val, arg);
              if(!dimension) {
                result.insert(result.end(), val.size() - 1,
                  static_cast<ScalarType>(1.) / val.size());
              } else {
                for(auto i = std::size_t(1); i < val.size(); ++i) {
                  if(i == *dimension) {
                    result.push_back(static_cast<ScalarType>(1.));
                  } else {
                    result.push_back(static_cast<ScalarType>(0.));
                  }
                }
              }
            } else {
              result.push_back(ScalarType{});
              Details::apply_basis(result.back(), m_to_negate[i + 1], arg, val);
            }
          }, identity);
        }
      });
    }, arguments);
    return result;
  }

  template<typename S, typename T>
  typename Basis<S, T>::ScalarSample Basis<S, T>::apply(const Sample& sample)
      const {
    auto arguments = apply(sample.m_arguments);
    auto result = result_cast(sample.m_result);
    return { result, arguments };
  }

  template<typename S, typename T>
  auto Basis<S, T>::restore_result(ScalarResult value) const {
    auto result = value * m_result;
    if(m_to_negate[0]) {
      return -result;
    } else {
      return result;
    }
  }

  template<typename S, typename T>
  std::vector<std::size_t> Basis<S, T>::find_categorical(const Arguments&
      args) {
    auto result = std::vector<std::size_t>();
    visit_arguments([&](const auto& value, auto index) {
      if(!is_continuous<ScalarType>(value)) {
        result.push_back(index);
      }
    }, args);
    return result;
  }

  template<typename S, typename T>
  template<typename Trial>
  void Basis<S, T>::compute_factors(const Trial& trial,
      const std::vector<std::size_t>& indexes) {
    for(auto& sample : trial) {
      auto last_factor_index = std::size_t(0);
      visit_arguments([&](const auto& value, auto index) {
        if(last_factor_index < indexes.size() && index ==
            indexes[last_factor_index]) {
          m_arguments.add_category(index, value);
          ++last_factor_index;
        }
      }, sample.m_arguments);
    }
  }

  template<typename S, typename T>
  typename Basis<S, T>::ScalarResult Basis<S, T>::result_cast(const Result&
      value) const {
    auto result = ScalarResult();
    Details::apply_basis(result, m_to_negate[0], value, m_result);
    return result;
  }
}

#endif
