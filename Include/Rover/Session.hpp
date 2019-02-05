#ifndef SESSION_HPP
#define SESSION_HPP

#include <optional>
#include "Reference.hpp"

namespace Rover {
namespace Session {

  template<typename R>
  struct Entry {
    static_assert(is_reference_v<R>,
      "Attempt to create a session Entry with a non-reference type");

    using Reference = R;
    using ValueType = typename Reference::Type;

    Reference reference;
    std::optional<ValueType> value;

    Entry(const Reference& param)
      : reference(param) {
    }

    Entry(const Reference& param, const ValueType& val)
      : reference(param),
        value(val) {
    }
  };
  
}
}

#endif