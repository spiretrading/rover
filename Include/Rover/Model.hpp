#ifndef ROVER_MODEL_HPP
#define ROVER_MODEL_HPP
#include <algorithm>
#include <numeric>
#include <random>
#include <type_traits>
#include <vector>
#include "Rover/Sample.hpp"
#include "Rover/ScalarView.hpp"

namespace {
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

namespace Rover { 

  //! Single-value regression (prediction) model.
  /*!
    \tparam A The type of the regression algorithm.
    \tparam T The type of the trial.
  */
  template<typename A, typename T>
  class Model {
    public:

      //! The type of the regression algorithm.
      using Algorithm = A;

      //! The computation type used by the algorithm.
      using ComputationType = typename Algorithm::Type;

      //! The type of the trial.
      using Trial = T;

      //! The type of a sample.
      using Sample = typename Trial::Sample;

      //! The type of a sample's arguments.
      using Arguments = typename Sample::Arguments;

      //! The type of a sample's result.
      using Result = typename Sample::Result;

      //! Creates a Model.
      /*!
        \param trial The trial.
        \param args The arguments for the algorithm's constructor.
      */
      template<typename... AlgoArgFwd>
      explicit Model(const Trial& trial, AlgoArgFwd&&... args);

      //! Predicts a Result for given arguments.
      auto operator ()(const Arguments& args) const;

    private:
      using ScalarSample = Rover::ScalarSample<ComputationType>;
      using ScalarArguments = typename ScalarSample::Arguments;
      using ScalarResult = typename ScalarSample::Result;

      struct Basis {
        Sample m_elements;
        std::vector<bool> m_to_negate;
      };

      Basis m_basis;
      Algorithm m_algorithm;

      static Basis compute_basis(const Trial& trial);
      ScalarSample sample_cast(const Sample& sample) const;
      ScalarArguments arguments_cast(const Arguments& sample) const;
      ScalarResult result_cast(const Result& value) const;
      auto retrieve_result(ScalarResult value) const;
  };

  template<typename A, typename T>
  template<typename... AlgoArgFwd>
  Model<A, T>::Model(const Trial& trial, AlgoArgFwd&&... args)
      : m_basis(compute_basis(trial)),
        m_algorithm(std::forward<AlgoArgFwd>(args)...) {
    auto view = ScalarView([&](std::size_t i) {
      return sample_cast(trial[i]);
    }, trial.size());
    m_algorithm.learn(std::move(view));
  }

  template<typename A, typename T>
  auto Model<A, T>::operator ()(const Arguments& args)
      const {
    auto arguments = arguments_cast(args);
    auto value = m_algorithm.predict(arguments);
    auto result = retrieve_result(value);
    return result;
  }

  template<typename A, typename T>
  typename Model<A, T>::Basis Model<A, T>::compute_basis(const Trial& trial) {
    auto order = std::vector<std::size_t>(trial.size());
    auto generator = std::mt19937(std::random_device()());
    std::iota(order.begin(), order.end(), std::size_t(0));
    auto basis = Basis{ trial[0], std::vector<bool>(1 + arguments_size(
      trial[0].m_arguments)) };
    auto& elements = basis.m_elements;
    auto& to_negate = basis.m_to_negate;
    visit_arguments([&](auto& result, auto i) {
      std::shuffle(order.begin(), order.end(), generator);
      const auto& first_arguments = trial[order[0]].m_arguments;
      visit_arguments([&](const auto& lhs, auto j) {
        if(i == j) {
          auto to_break = false;
          for(auto l = std::size_t(1); l < order.size() && !to_break; ++l) {
            const auto& second_arguments = trial[order[l]].m_arguments;
            visit_arguments([&](const auto& rhs, auto k) {
              if(j == k && solve_basis(result, to_negate[i + 1], lhs, rhs)) {
                to_break = true;
              }
            }, second_arguments);
          }
        }
      }, first_arguments);
    }, elements.m_arguments);
    std::shuffle(order.begin(), order.end(), std::move(generator));
    const auto& first_result = trial[order[0]].m_result;
    for(auto l = std::size_t(1); l < order.size(); ++l) {
      const auto& second_result = trial[order[l]].m_result;
      if(first_result != second_result) {
        solve_basis(elements.m_result, to_negate[0], first_result, second_result);
      }
    }
    return basis;
  }
  
  template<typename A, typename T>
  typename Model<A, T>::ScalarSample Model<A, T>::sample_cast(
      const Sample& sample) const {
    auto arguments = arguments_cast(sample.m_arguments);
    auto result = result_cast(sample.m_result);
    return { result, arguments };
  }

  template<typename A, typename T>
  typename Model<A, T>::ScalarArguments Model<A, T>::arguments_cast(
      const Arguments& arguments) const {
    auto result = ScalarArguments(arguments_size(arguments));
    visit_arguments([&](const auto& arg, auto i) {
      visit_arguments([&](const auto& identity, auto j) {
        if(i == j) {
          apply_basis(result[i], m_basis.m_to_negate[i + 1], arg, identity);
        }
      }, m_basis.m_elements.m_arguments);
    }, arguments);
    return result;
  }

  template<typename A, typename T>
  typename Model<A, T>::ScalarResult Model<A, T>::result_cast(
      const Result& value) const {
    auto result = ScalarResult();
    apply_basis(result, m_basis.m_to_negate[0], value,
      m_basis.m_elements.m_result);
    return result;
  }

  template<typename A, typename T>
  auto Model<A, T>::retrieve_result(ScalarResult value) const {
    auto result = value * m_basis.m_elements.m_result;
    if(m_basis.m_to_negate[0]) {
      return -result;
    } else {
      return result;
    }
  }
}

#endif
