#ifndef ROVER_BOX_HPP
#define ROVER_BOX_HPP
#include <memory>
#include <type_traits>
#include "Rover/Evaluator.hpp"
#include "Rover/Noncopyable.hpp"
#include "Rover/Pointer.hpp"

namespace Rover {

  //! Provides type-erasure over generators evaluating to a type T.
  /*!
    \tparam T The type of value to generate.
  */
  template<typename T>
  class Box : private Noncopyable {
    public:

      //! The type of value to generate.
      using Type = T;

      //! Packs a generator or generator pointer into a Box
      template<typename GeneratorFwd, std::enable_if_t<!std::is_same_v<
          std::decay_t<GeneratorFwd>, Box>>* = nullptr>
      explicit Box(GeneratorFwd&& gen);

      Type generate(Evaluator& evaluator);

    private:

      struct WrapperBase {
        virtual ~WrapperBase() = default;
        virtual Type generate(Evaluator& evaluator) = 0;
      };

      template<typename Generator>
      struct ValueWrapper;

      template<typename GeneratorFwd>
      ValueWrapper(GeneratorFwd&&) -> ValueWrapper<std::decay_t<GeneratorFwd>>;

      template<typename Generator>
      struct PointerWrapper;

      template<typename GeneratorFwd>
      PointerWrapper(GeneratorFwd&&)->PointerWrapper<std::decay_t<GeneratorFwd>>;

      std::unique_ptr<WrapperBase> m_generator;
  };

  template<typename GeneratorFwd>
  Box(GeneratorFwd&&) -> Box<GeneratorValueType<std::decay_t<GeneratorFwd>>>;

  template<typename T>
  template<typename GeneratorFwd, std::enable_if_t<!std::is_same_v<
    std::decay_t<GeneratorFwd>, Box<T>>>*>
  Box<T>::Box(GeneratorFwd&& gen)
      : m_generator([&gen]() mutable {
          if constexpr(is_object_pointer_v<std::decay_t<GeneratorFwd>>) {
            return std::make_unique<PointerWrapper<std::decay_t<GeneratorFwd>>>(
                std::forward<GeneratorFwd>(gen));
          } else {
            return std::make_unique<ValueWrapper<std::decay_t<GeneratorFwd>>>(
                std::forward<GeneratorFwd>(gen));
          }
        }()) {}

  template<typename T>
  T Box<T>::generate(Evaluator& evaluator) {
    return m_generator->generate(evaluator);
  }

  template<typename T>
  template<typename Generator>
  struct Box<T>::ValueWrapper : public WrapperBase {
    template<typename GeneratorFwd, std::enable_if_t<!std::is_same_v<
        std::decay_t<GeneratorFwd>, ValueWrapper>>* = nullptr>
    ValueWrapper(GeneratorFwd&& gen)
        : m_generator(std::forward<GeneratorFwd>(gen)) {}

    virtual T generate(Evaluator& evaluator) override final {
      return evaluator.evaluate(m_generator);
    }

    Generator m_generator;
  };

  template<typename T>
  template<typename Generator>
  struct Box<T>::PointerWrapper : public WrapperBase {
    template<typename GeneratorFwd, std::enable_if_t<!std::is_same_v<
        std::decay_t<GeneratorFwd>, PointerWrapper>>* = nullptr>
    PointerWrapper(GeneratorFwd&& gen)
        : m_generator(std::forward<GeneratorFwd>(gen)) {}

    virtual T generate(Evaluator& evaluator) override final {
      return evaluator.evaluate(*m_generator);
    }

    Generator m_generator;
  };
}

#endif
