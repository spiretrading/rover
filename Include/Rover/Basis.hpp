#ifndef ROVER_BASIS_HPP
#define ROVER_BASIS_HPP
#include <algorithm>
#include <numeric>
#include <random>
#include <vector>
#include "Rover/Sample.hpp"
#include "Rover/ScalarView.hpp"

namespace Rover {

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
      Sample m_elements;
      std::vector<bool> m_to_negate;

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

  template<typename C, typename V, typename B>
  constexpr std::enable_if_t<!std::is_same_v<V, B>> apply_basis(C&,
    bool to_negate, const V&, const B&) {}

  template<typename C, typename V, typename B>
  constexpr std::enable_if_t<std::is_same_v<V, B>> apply_basis(C& c,
      bool to_negate, const V& v, const B& b) {
    if(to_negate) {
      c = -static_cast<C>(v / b);
    } else {
      c = static_cast<C>(v / b);
    }
  }
}

  template<typename S, typename T>
  template<typename Trial>
  Basis<S, T>::Basis(const Trial& trial)
      : m_elements(trial[0]),
        m_to_negate(1 + arguments_size(trial[0].m_arguments)) {
    auto order = std::vector<std::size_t>(trial.size());
    auto generator = std::mt19937(std::random_device()());
    std::iota(order.begin(), order.end(), std::size_t(0));
    visit_arguments([&](auto& result, auto i) {
      std::shuffle(order.begin(), order.end(), generator);
      const auto& first_arguments = trial[order[0]].m_arguments;
      visit_arguments([&](const auto& lhs, auto j) {
        if(i == j) {
          auto to_break = false;
          for(auto l = std::size_t(1); l < order.size() && !to_break; ++l) {
            const auto& second_arguments = trial[order[l]].m_arguments;
            visit_arguments([&](const auto& rhs, auto k) {
              if(j == k && Details::solve_basis(result, m_to_negate[i + 1],
                  lhs, rhs)) {
                to_break = true;
              }
            }, second_arguments);
          }
        }
      }, first_arguments);
    }, m_elements.m_arguments);
    std::shuffle(order.begin(), order.end(), std::move(generator));
    const auto& first_result = trial[order[0]].m_result;
    for(auto l = std::size_t(1); l < order.size(); ++l) {
      const auto& second_result = trial[order[l]].m_result;
      if(first_result != second_result) {
        Details::solve_basis(m_elements.m_result, m_to_negate[0], first_result,
          second_result);
      }
    }
  }
  
  template<typename S, typename T>
  typename Basis<S, T>::ScalarArguments Basis<S, T>::apply(const Arguments&
      arguments) const {
    auto result = ScalarArguments(arguments_size(arguments));
    visit_arguments([&](const auto& arg, auto i) {
      visit_arguments([&](const auto& identity, auto j) {
        if(i == j) {
          Details::apply_basis(result[i], m_to_negate[i + 1], arg, identity);
        }
      }, m_elements.m_arguments);
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
    auto result = value * m_elements.m_result;
    if(m_to_negate[0]) {
      return -result;
    } else {
      return result;
    }
  }

  template<typename S, typename T>
  typename Basis<S, T>::ScalarResult Basis<S, T>::result_cast(const Result&
      value) const {
    auto result = ScalarResult();
    Details::apply_basis(result, m_to_negate[0], value, m_elements.m_result);
    return result;
  }
}

#endif
