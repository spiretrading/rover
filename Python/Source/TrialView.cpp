#include "TrialView.hpp"

void Rover::export_trial_view(pybind11::module& module) {
  export_trial_view<PythonSample>(module, "");
}
