#include "Sample.hpp"

using namespace pybind11;

namespace Rover {
  void export_sample(module& module) {
    class_<PythonSample>(module, "Sample")
      .def(init<>())
      .def_readwrite("result", &PythonSample::m_result)
      .def_readwrite("arguments", &PythonSample::m_arguments);
  }
}
