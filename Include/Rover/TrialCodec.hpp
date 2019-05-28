#ifndef ROVER_TRIAL_CODEC_HPP
#define ROVER_TRIAL_CODEC_HPP
#include <iostream>

namespace Rover {

  //! Saves a trial to a CSV, separating samples by line breaks.
  /*!
    \param Trial The trial to save.
    \param sink The output stream.
  */
  template<typename Trial>
  void save_to_csv(const Trial& trial, std::ostream& sink);

  //! Loads a trial from a CSV, discarding previously stored samples.
  /*!
    \param source The input stream containing a CSV-encoded trial.
    \param trial The resulting trial.
  */
  template<typename Trial>
  void load_from_csv(std::istream& source, Trial& trial);

  template<typename Trial>
  void save_to_csv(const Trial& trial, std::ostream& sink) {
    for(auto& sample : trial) {
      sink << sample << '\n';
    }
  }

  template<typename Trial>
  void load_from_csv(std::istream& source, Trial& trial) {
    auto result = Trial();
    while(source.good()) {
      auto sample = typename Trial::Sample();
      source >> sample;
      if(source.good()) {
        result.insert(std::move(sample));
      }
    };
    trial = std::move(result);
  }
}

#endif
