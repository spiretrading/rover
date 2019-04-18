#include "TrialView.hpp"

namespace Rover {

  void export_trial_view(pybind11::module& module) {
    export_trial_view<PythonSample>(module, "");
  }
}
