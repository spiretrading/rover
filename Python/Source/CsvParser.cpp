#include "CsvParser.hpp"

using namespace pybind11;
using namespace Rover;

namespace {
  class ParameterWrapper {
    public:
      object get(object parser) {
        return parser(m_str);
      }

    private:
      friend std::istream& operator >>(std::istream& stream, ParameterWrapper&
        wrapper);

      std::string m_str;
  };

  std::istream& operator >>(std::istream& stream, ParameterWrapper&
      wrapper) {
    stream >> wrapper.m_str;
    return stream;
  }
}

void Rover::export_save_to_csv(module& module) {
  export_save_to_csv<PythonSample>(module, "save_to_csv");
}

void Rover::export_load_from_csv(module& module) {
  module.def("load_from_csv", [](object& source, object& trial, tuple
      parsers) {
    auto line = std::string();
    while(!(line = source.attr("readline")().cast<std::string>()).empty()) {
      auto stream = std::istringstream(line);
      auto sample = PythonSample();
      auto parse_parameter = [&](auto parser_index) -> std::optional<object> {
        if(!stream.good()) {
          return std::nullopt;
        }
        auto wrapper = ParameterWrapper();
        get_next_cvs_field(stream, wrapper);
        if(!stream) {
          return std::nullopt;
        }
        return wrapper.get(parsers[parser_index]);
      };
      auto result = parse_parameter(0);
      if(!result) {
        return;
      }
      sample.m_result = *result;
      sample.m_arguments = tuple(parsers.size() - 1);
      for(auto i = size_t(1); i < parsers.size(); ++i) {
        auto argument = parse_parameter(i);
        if(!argument) {
          return;
        }
        sample.m_arguments[i - 1] = *argument;
      }
      trial.attr("insert")(sample);
    }
  });
}
