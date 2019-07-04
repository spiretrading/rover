#include "Rover/Python/Filter.hpp"
#include "Rover/Python/Autobox.hpp"

using namespace pybind11;
using namespace Rover;

void Rover::export_filter(module& module) {
  using FilterType = Filter<std::function<bool (object)>, Box<object>>;
  class_<FilterType>(module, "Filter")
    .def(init([](object predicate, object generator) {
       return FilterType([predicate = std::move(predicate)](object obj) {
         return predicate.attr("__call__")(std::move(obj)).cast<bool>();
       }, python_autobox<object>(std::move(generator)));
     }))
    .def(init([](object filter) {
       return FilterType([](object) {
         return true;
       }, python_autobox<object>(std::move(filter)));
     }))
    .def("generate", &FilterType::generate);
  implicitly_convertible<FilterType, Box<object>>();
}
