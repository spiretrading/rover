#ifndef ROVER_PYTHON_TRIAL_CODEC_HPP
#define ROVER_PYTHON_TRIAL_CODEC_HPP
#include <pybind11/iostream.h>
#include <pybind11/pybind11.h>
#include "Rover/TrialCodec.hpp"
#include "TrialView.hpp"

namespace Rover {

  //! Exports save_to_csv for PythonSample Trials.
  /*!
    \param module The module to export the function to.
  */
  void export_save_to_csv(pybind11::module& module);

  //! Exports load_from_csv for PythonSample Trials.
  /*!
    \param module The module to export the function to.
  */
  void export_load_from_csv(pybind11::module& module);

  //! Exports save_to_csv for a given Sample type.
  /*!
    \tparam Sample The type of the samples.
    \param module The module to export the function to.
    \param name The name of the exported function.
  */
  template<typename Sample>
  void export_save_to_csv(pybind11::module& module, const char* name);

  //! Exports load_from_csv for a given Sample type.
  /*!
    \tparam Sample The type of the samples.
    \param module The module to export the function to.
    \param name The name of the exported function.
  */
  template<typename Sample>
  void export_load_from_csv(pybind11::module& module, const char* name);
  
  template<typename Sample>
  void export_save_to_csv(pybind11::module& module, const char* name) {
    module.def(name, [](const TrialView<Sample>& trial, pybind11::object& 
        sink) {
      auto stream = std::ostringstream();
      auto redirect = pybind11::scoped_ostream_redirect(stream, sink);
      save_to_csv(trial, stream);
    });
  }

  template<typename Sample>
  void export_load_from_csv(pybind11::module& module, const char* name) {
    module.def(name, [](pybind11::object& source, pybind11::object& trial) {
      auto line = std::string();
      while(!(line = source.attr("readline")().cast<std::string>()).empty()) {
        auto stream = std::istringstream(line);
        auto sample = Sample();
        stream >> sample;
        trial.attr("insert")(sample);
      }
    });
  }
}

#endif
