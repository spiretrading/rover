#ifndef ROVER_PYTHON_TRIAL_VIEW_HPP
#define ROVER_PYTHON_TRIAL_VIEW_HPP
#include <functional>
#include <string_view>
#include <type_traits>
#include <pybind11/pybind11.h>
#include "Rover/ListTrial.hpp"
#include "Rover/TrialView.hpp"
#include "Sample.hpp"

namespace Rover {
  namespace Details {
    class NativeTrialProxy {
      public:
        using Sample = PythonSample;
        using SampleGetter = std::function<Sample (std::size_t)>;

        class ConstIterator {
          public:
            const Sample& operator [](std::ptrdiff_t offset) {
              m_sample = m_getter(m_offset + offset);
              return m_sample;
            }

          private:
            friend class NativeTrialProxy;

            const SampleGetter& m_getter;
            std::size_t m_offset;
            Sample m_sample;

            ConstIterator(const SampleGetter& getter, std::size_t offset)
              : m_getter(getter),
                m_offset(offset) {}
        };
    
        NativeTrialProxy(const pybind11::object& t)
          : m_size(t.attr("__len__")().cast<std::size_t>()),
            m_getter([t](std::size_t index) {
              auto sample = t.attr("__getitem__")(index);
              auto result = SampleConverter::get_instance().cast(sample);
              return result;
            }) {}
    
        std::size_t size() const {
          return m_size;
        }
    
        ConstIterator begin() const {
          return ConstIterator(m_getter, 0);
        }
    
      private:
        std::size_t m_size;
        SampleGetter m_getter;
    };

    template<typename S>
    class PythonTrialProxy {
      public:
        using Sample = S;

        class ConstIterator {
          public:
            const Sample& operator [](std::ptrdiff_t offset) {
              auto python_sample = m_view[m_offset + offset];
              m_sample = sample_cast<Sample>(python_sample);
              return m_sample;
            }

          private:
            friend class PythonTrialProxy;

            const TrialView<PythonSample>& m_view;
            std::size_t m_offset;
            Sample m_sample;

            ConstIterator(const TrialView<PythonSample>& view, std::size_t
                offset)
              : m_view(view),
                m_offset(offset) {}
        };
        
        PythonTrialProxy(const TrialView<PythonSample>& t)
          : m_view(t) {}

        std::size_t size() const {
          return m_view.size();
        }

        ConstIterator begin() const {
          return ConstIterator(m_view, 0);
        }

      private:
        const TrialView<PythonSample>& m_view;
        Sample m_sample;
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
