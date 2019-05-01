#ifndef ROVER_PYTHON_TRIAL_VIEW_HPP
#define ROVER_PYTHON_TRIAL_VIEW_HPP
#include <functional>
#include <string_view>
#include <type_traits>
#include <pybind11/pybind11.h>
#include "Rover/ListTrial.hpp"
#include <Rover/TrialIterator.hpp>
#include "Rover/TrialView.hpp"
#include "Sample.hpp"

namespace Rover {
namespace Details {
  class NativeTrialProxy {
    public:
      using Sample = PythonSample;
      using SampleGetter = std::function<Sample (std::size_t)>;
      using Iterator = TrialIterator<NativeTrialProxy>;
  
      NativeTrialProxy(const pybind11::object& view)
        : m_view(view),
          m_size(m_view.attr("__len__")().cast<std::size_t>()) {}
  
      std::size_t size() const {
        return m_size;
      }
  
      Sample operator [](std::size_t index) const {
        auto sample = m_view.attr("__getitem__")(index);
        auto result = SampleConverter::get_instance().cast(sample);
        return result;
      }

      Iterator begin() const {
        return Iterator(*this, 0);
      }

      Iterator end() const {
        return Iterator(*this, size());
      }
  
    private:
      pybind11::object m_view;
      std::size_t m_size;
  };

  template<typename S>
  class PythonTrialProxy {
    public:
      using Sample = S;
      using Iterator = TrialIterator<PythonTrialProxy>;
      
      PythonTrialProxy(const TrialView<PythonSample>& t)
        : m_view(&t) {}

      std::size_t size() const {
        return m_view->size();
      }

      Sample operator [](std::size_t index) const {
        auto python_sample = (*m_view)[index];
        auto result = sample_cast<Sample>(python_sample);
        return result;
      }

      Iterator begin() const {
        return Iterator(*this, 0);
      }

      Iterator end() const {
        return Iterator(*this, size());
      }

    private:
      const TrialView<PythonSample>* m_view;
  };
}

  //! Exports the TrialView for PythonSample.
  /*!
    \param module The module to export the class to.
  */
  void export_trial_view(pybind11::module& module);

  //! Exports the TrialView for a particular Sample type.
  /*!
    \param module The module to export the class to.
    \param type_name The name of the Sample type.
  */
  template<typename S>
  void export_trial_view(pybind11::module& module, std::string_view type_name);

  template<typename S>
  void export_trial_view(pybind11::module& module, std::string_view
      type_name) {
    auto proxy_name = std::string("_PythonTrialProxy").append(type_name);
    pybind11::class_<Details::PythonTrialProxy<S>>(module, proxy_name.c_str())
      .def(pybind11::init([](const TrialView<PythonSample>& v) {
         return Details::PythonTrialProxy<S>(v);
       }), pybind11::keep_alive<2, 1>());
    pybind11::implicitly_convertible<TrialView<PythonSample>,
      Details::PythonTrialProxy<S>>();
    auto name = std::string("TrialView").append(type_name);
    pybind11::class_<TrialView<S>>(module, name.c_str())
      .def(pybind11::init<const ListTrial<S>&>())
      .def(pybind11::init([](const Details::PythonTrialProxy<S>& p) {
         return TrialView<S>(p);
       }), pybind11::keep_alive<2, 1>())
      .def("__getitem__", &TrialView<S>::operator [])
      .def("__iter__", [](const TrialView<S>& v) {
         return pybind11::make_iterator(v.begin(), v.end());
       })
      .def("__len__", &TrialView<S>::size);
    pybind11::implicitly_convertible<TrialView<S>, Details::NativeTrialProxy>();
    pybind11::implicitly_convertible<TrialView<S>, TrialView<PythonSample>>();
    pybind11::implicitly_convertible<TrialView<PythonSample>, TrialView<S>>();
  }
}

#endif
