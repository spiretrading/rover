#ifndef SESSION_VISITOR_HPP
#define SESSION_VISITOR_HPP

#include <functional>
#include <tuple>
#include <type_traits>
#include <variant>
#include "Definitions.hpp"
#include "Utilities.hpp"

namespace Rover {
  namespace Session {

    namespace Details {

      template<typename Session>
      struct return_values;

      template<typename... Args>
      struct return_values<std::tuple<Args...>> {
        using type = std::tuple<return_value_t<Args>...>;
      };

      template<typename Session>
      using return_values_t = typename return_values<Session>::type;

      template<typename Session>
      using impl_return_t = to_variant_t<remove_duplicates_t<return_values_t<
        Session>>>;

      template<typename SessionType, size_t I = 0, typename = void>
      struct VisitorImpl {
        static_assert(Session::is_entry_v<std::tuple_element_t<I, SessionType>>,
          "Non-Entry type found in Session");

        template<typename Generator>
        impl_return_t<SessionType>
        operator ()(SessionType& session,
          const Reference<Generator>& reference) {
          auto& entry = std::get<I>(session);
          if(reference.is_same(entry.reference)) {
            if(!entry.value) {
              entry.value = entry.reference.generate_impl(session);
            }

            return *entry.value;
          }

          return VisitorImpl<SessionType, I + 1>()(session, reference);
        }
      };

      template<typename SessionType, size_t I>
      struct VisitorImpl<SessionType, I, std::enable_if_t<I ==
        std::tuple_size_v<SessionType>>> {
        template<typename Generator>
        impl_return_t<SessionType>
        operator ()(SessionType& session,
          const Reference<Generator>& reference) {
          return reference.generate_impl(session);
        }
      };
    }

    template<typename SessionType>
    struct Visitor {
      template<typename Generator>
      typename Generator::Type operator ()(SessionType& session,
        const Reference<Generator>& reference) {
        auto var = Details::VisitorImpl<SessionType>()(session, reference);
        auto result = std::get_if<typename Generator::Type>(&var);
        if(result == nullptr) {
          throw std::bad_variant_access();
        }
        return *result;
      }
    };

    template<>
    struct Visitor<std::tuple<>> {
      template<typename Generator>
      typename Generator::Type operator ()(std::tuple<>& session,
        const Reference<Generator>& reference) {
        return reference.generate_impl(session);
      }
    };
  }
}

#endif