#ifndef ROVER_MODEL_HPP
#define ROVER_MODEL_HPP
#include <algorithm>
#include <numeric>
#include <random>
#include <type_traits>
#include <vector>
#include "ScalarView.hpp"

namespace {
  template<typename R, typename X, typename Y>
  constexpr std::enable_if_t<!std::is_same_v<std::decay_t<R>,
      std::decay_t<X>> || !std::is_same_v<std::decay_t<R>, std::decay_t<Y>>,
      bool> solve_basis(R&, X&, Y&) {
    return false;
  }

  template<typename R, typename X, typename Y>
  std::enable_if_t<std::is_same_v<std::decay_t<R>, std::decay_t<X>> &&
      std::is_same_v<std::decay_t<R>, std::decay_t<Y>>, bool>
      solve_basis(R& r, X& x, Y& y) {
    if(x == y) {
      return false;
    }
    if(x + y == x) {
      r = x;
    }
    else {
      r = y;
    }
    return true;
  }

  template<typename C, typename V, typename B>
  constexpr std::enable_if_t<!std::is_same_v<V, B>> apply_basis(C&, const V&,
    const B&) {}

  template<typename C, typename V, typename B>
  constexpr std::enable_if_t<std::is_same_v<V, B>> apply_basis(C& c,
      const V& v, const B& b) {
    c = v / b;
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
      using CompType = typename Algorithm::CompType;

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
      using Reader = ScalarView<CompType>;
      using ReaderSample = typename Reader::Sample;
      using ReaderArguments = typename Reader::Sample::Arguments;
      using ReaderResult = typename Reader::Sample::Result;

      Sample m_basis;
      Algorithm m_algorithm;

      static Sample compute_basis(const Trial& trial);
      ReaderSample sample_cast(const Sample& sample) const;
      ReaderArguments arguments_cast(const Arguments& sample) const;
      ReaderResult result_cast(const Result& value) const;
      auto result_cast(ReaderResult value) const;
  };

  template<typename A, typename T>
  template<typename... AlgoArgFwd>
  Model<A, T>::Model(const Trial& trial, AlgoArgFwd&&... args)
    : m_basis(compute_basis(trial)),
      m_algorithm(std::forward<AlgoArgFwd>(args)...) {
    auto reader = ScalarView<CompType>([&](std::size_t i) {
      return sample_cast(trial[i]);
    }, trial.size());
    m_algorithm.learn(std::move(reader));
  }

  template<typename A, typename T>
  auto Model<A, T>::operator ()(const Arguments& args)
      const {
    auto arguments = arguments_cast(args);
    auto value = m_algorithm.predict(arguments);
    auto result = result_cast(value);
    return result;
  }

  template<typename A, typename T>
  typename Model<A, T>::Sample Model<A, T>::compute_basis(const Trial& trial) {
    auto order = std::vector<std::size_t>(trial.size());
    auto generator = std::mt19937(std::random_device()());
    std::iota(order.begin(), order.end(), std::size_t(0));
    auto basis = trial[0];
    visit_arguments([&](auto& result, auto i) {
      std::shuffle(order.begin(), order.end(), generator);
      const auto& first_arguments = trial[order[0]].m_arguments;
      visit_arguments([&](const auto& lhs, auto j) {
        if(i == j) {
          auto to_break = false;
          for(auto l = std::size_t(1); l < order.size() && !to_break; ++l) {
            const auto& second_arguments = trial[order[l]].m_arguments;
            visit_arguments([&](const auto& rhs, auto k) {
              if(j == k && solve_basis(result, lhs, rhs)) {
                to_break = true;
              }
            }, second_arguments);
          }
        }
      }, first_arguments);
    }, basis.m_arguments);
    std::shuffle(order.begin(), order.end(), std::move(generator));
    const auto& first_result = trial[order[0]].m_result;
    for(auto l = std::size_t(1); l < order.size(); ++l) {
      const auto& second_result = trial[order[l]].m_result;
      if(first_result != second_result) {
        solve_basis(basis.m_result, first_result, second_result);
      }
    }
    return basis;
  }
  
  template<typename A, typename T>
  typename Model<A, T>::ReaderSample Model<A, T>::sample_cast(
      const Sample& sample) const {
    auto arguments = arguments_cast(sample.m_arguments);
    auto result = result_cast(sample.m_result);
    return { result, arguments };
  }

  template<typename A, typename T>
  typename Model<A, T>::ReaderArguments Model<A, T>::arguments_cast(
      const Arguments& arguments) const {
    auto result = std::vector<CompType>(arguments_size(arguments));
    visit_arguments([&](const auto& arg, auto i) {
      visit_arguments([&](const auto& identity, auto j) {
        if(i == j) {
          apply_basis(result[i], arg, identity);
        }
      }, m_basis.m_arguments);
    }, arguments);
    return result;
  }

  template<typename A, typename T>
  typename Model<A, T>::ReaderResult Model<A, T>::result_cast(
      const Result& value) const {
    return value / m_basis.m_result;
  }

  template<typename A, typename T>
  auto Model<A, T>::result_cast(ReaderResult value)
      const {
    return m_basis.m_result * value;
  }
}

#endif
