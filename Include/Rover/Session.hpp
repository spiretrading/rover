#ifndef ROVER_SESSION_HPP
#define ROVER_SESSION_HPP
#include <optional>
#include "Reference.hpp"

namespace Rover {

  template<typename R>
  struct SessionEntry {
    static_assert(is_reference_v<R>,
        "Attempt to create a session Entry with a non-reference type");

    using Reference = R;
    using ValueType = typename Reference::Type;

    Reference reference;
    std::optional<ValueType> value;

    SessionEntry(const Reference& param)
        : reference(param) {
    }

    SessionEntry(const Reference& param, const ValueType& val)
        : reference(param),
          value(val) {
    }
  };
}

#endif
