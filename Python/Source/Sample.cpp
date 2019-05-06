#include "Sample.hpp"

using namespace pybind11;
using namespace Rover;

std::size_t Rover::ArgumentVisitor<PythonSample::Arguments>::size(const
    Arguments& tuple) {
  return tuple.size();
}

void Rover::export_sample(module& module) {
  class_<PythonSample>(module, "Sample")
    .def(init<>())
    .def(init<PythonSample::Result, PythonSample::Arguments>())
    .def(init([](const object& obj) {
       return Details::SampleConverter::get_instance().cast(obj);
     }))
    .def_readwrite("result", &PythonSample::m_result)
    .def_readwrite("arguments", &PythonSample::m_arguments);
}
