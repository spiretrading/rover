if(WIN32)
  include("${CMAKE_CURRENT_LIST_DIR}/dependencies.windows.cmake")
else()
  include("${CMAKE_CURRENT_LIST_DIR}/dependencies.posix.cmake")
endif()
set(ROVER_INCLUDE_PATH "${CMAKE_CURRENT_LIST_DIR}/Include")
set(CATCH_INCLUDE_PATH
  "${PROJECT_BINARY_DIR}/Dependencies/Catch2-2.2.1/single_include")
set(PYBIND11_INCLUDE_PATH
  "${PROJECT_BINARY_DIR}/Dependencies/pybind11-2.2.4/include")
set(PYTHON_INCLUDE_PATH
  "${PROJECT_BINARY_DIR}/Dependencies/Python-3.7.2/Include")
