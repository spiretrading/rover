#include <sstream>
#include "Sample.hpp"

using namespace pybind11;
using namespace Rover;

std::size_t Rover::ArgumentVisitor<PythonSample::Arguments>::size(const
    Arguments& tuple) {
  return tuple.size();
}

std::ostream& Rover::operator <<(std::ostream& stream, const PythonSample&
    sample) {
  stream << '(' << str(sample.m_result).cast<std::string>();
  for(auto& arg : sample.m_arguments) {
    stream << ", " << str(arg).cast<std::string>();
  }
  stream << ')';
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
    .def_readwrite("arguments", &PythonSample::m_arguments)
    .def("__str__", [](const PythonSample& sample) {
       auto stream = std::ostringstream();
       stream << sample;
       return stream.str();
     });
}
