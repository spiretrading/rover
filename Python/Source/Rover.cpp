#include <pybind11/pybind11.h>
#include "Box.hpp"
#include "Constant.hpp"
#include "Evaluator.hpp"
#include "Lift.hpp"
#include "ListTrial.hpp"
#include "Range.hpp"
#include "Sample.hpp"
#include "TrialView.hpp"

using namespace pybind11;
using namespace Rover;

PYBIND11_MODULE(rover, module) {
  export_evaluator(module);
  export_box(module);
  export_constant(module);
  export_range(module);
  export_lift(module);
  export_sample(module);
  export_sample<float, float, float>(module, "Float");
  export_list_trial(module);
  export_list_trial<Sample<float, float, float>>(module, "Float");
  export_trial_view(module);
  export_trial_view<Sample<float, float, float>>(module, "Float");
}
