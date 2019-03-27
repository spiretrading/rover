#include "Sample.hpp"

using namespace pybind11;
using namespace Rover;

void Rover::export_sample(module& module) {
  class_<PythonSample>(module, "Sample")
    .def(init<>())
    .def(init<PythonSample::Result, PythonSample::Parameters>())
    .def_readwrite("result", &PythonSample::m_result)
    .def_readwrite("arguments", &PythonSample::m_arguments);
}
