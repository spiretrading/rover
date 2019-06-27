#ifndef ROVER_EVALUATION_HPP
#define ROVER_EVALUATION_HPP
#include <algorithm>
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <type_traits>
#include <utility>
#include <vector>

namespace Rover {

  /** Encapsulates the state needed to evaluate a generator. */
  class Evaluator {
    public:

      //! Returns the argument evaluated by a generator.
      /*!
        \param generator The generator to evaluate.
        \return The argument generated.
      */
      template<typename Generator>
      typename Generator::Type evaluate(Generator& generator);

    private:
      struct BaseEvaluation {
        virtual ~BaseEvaluation() = default;
        virtual bool is_equal(const std::type_index& type,
          const void* address) = 0;
      };
      template<typename T>
      struct ValueEvaluation : BaseEvaluation {
        using Type = T;
        Type m_value;

        template<typename U>
        ValueEvaluation(U&& value);
      };
      template<typename T, typename G>
      struct Evaluation : ValueEvaluation<T> {
        using Type = typename ValueEvaluation<T>::Type;
        using Generator = G;
        const Generator* m_generator;

        template<typename U>
        Evaluation(U&& value, const Generator& generator);
        bool is_equal(const std::type_index& type,
          const void* address) override;
      };
      std::vector<std::unique_ptr<BaseEvaluation>> m_evaluations;
  };

  //! Tests if two instances represent the same effective generator.
  template<typename Generator>
  bool is_same(const Generator& left, const Generator& right) {
    return &left == &right;
  }

  template<typename T>
  template<typename U>
  Evaluator::ValueEvaluation<T>::ValueEvaluation(U&& value)
    : m_value(std::forward<U>(value)) {}

  template<typename T, typename G>
  template<typename U>
  Evaluator::Evaluation<T, G>::Evaluation(U&& value, const Generator& generator)
      : ValueEvaluation(std::forward<U>(value)),
        m_generator(&generator) {}

  template<typename T, typename G>
  bool Evaluator::Evaluation<T, G>::is_equal(const std::type_index& type,
      const void* address) {
    return type == typeid(G) && is_same(*m_generator,
      *static_cast<const Generator*>(address));
  }

  template<typename Generator>
  typename Generator::Type Evaluator::evaluate(Generator& generator) {
    using Type = typename Generator::Type;
    auto i = std::find_if(m_evaluations.begin(), m_evaluations.end(),
      [&](const auto& evaluation) {
        return evaluation->is_equal(typeid(Generator), &generator);
      });
    if(i == m_evaluations.end()) {
      auto evaluation = std::make_unique<Evaluation<Type, Generator>>(
        generator.generate(*this), generator);
      m_evaluations.push_back(std::move(evaluation));
      i = m_evaluations.end() - 1;
    }
    return static_cast<ValueEvaluation<Type>&>(**i).m_value;
  }
}

#endif
