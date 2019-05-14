#include "Sample.hpp"

using namespace pybind11;
using namespace Rover;

std::size_t Rover::ArgumentVisitor<PythonSample::Arguments>::size(const
    Arguments& tuple) {
  return tuple.size();
}

std::ostream& Rover::operator <<(std::ostream& stream, const PythonSample&
    sample) {
  stream << sample.m_result.attr("__str__").cast<std::string>();
  for(auto& arg : sample.m_arguments) {
    stream << ',' << arg.attr("__str__").cast<std::string>();
  }
  return stream;
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
