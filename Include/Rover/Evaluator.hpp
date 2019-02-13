#ifndef ROVER_EVALUATION_HPP
#define ROVER_EVALUATION_HPP
#include <memory>
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
      struct BaseEntry {
        virtual ~BaseEntry() = default;
      };
      template<typename T>
      struct Entry : BaseEntry {
        using Type = T;
        Type m_value;

        template<typename U>
        Entry(U&& value);
      };
      struct Evaluation {
        void* m_generator;
        std::unique_ptr<BaseEntry> m_entry;

        template<typename Generator>
        Evaluation(Generator& generator, std::unique_ptr<BaseEntry> entry);
      };
      std::vector<Evaluation> m_evaluations;
  };

  template<typename T>
  template<typename U>
  Evaluator::Entry<T>::Entry(U&& value)
      : m_value(std::forward<U>(value)) {}

  template<typename Generator>
  Evaluator::Evaluation::Evaluation(Generator& generator,
      std::unique_ptr<BaseEntry> entry)
      : m_generator(&generator),
        m_entry(std::move(entry)) {}

  template<typename Generator>
  typename Generator::Type Evaluator::evaluate(Generator& generator) {
    using Type = typename Generator::Type;
    auto i = std::find_if(m_evaluations.begin(), m_evaluations.end(),
      [&] (const auto& evaluation) {
        return evaluation.m_generator == &generator;
      });
    if(i == m_evaluations.end()) {
      auto entry = std::make_unique<Entry<Type>>(generator.generate(*this));
      m_evaluations.emplace_back(generator, std::move(entry));
      i = m_evaluations.end() - 1;
    }
    return static_cast<Entry<Type>&>(*i->m_entry).m_value;
  }
}

#endif
