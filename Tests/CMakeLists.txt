include_directories(SYSTEM ${CATCH2_INCLUDE_PATH})
file(GLOB source_files ${ROVER_INCLUDE_PATH}/../Tests/Source/*.cpp)
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif()
add_executable(rover_tester ${source_files})
target_link_libraries(rover_tester
  debug ${DLIB_LIBRARY_DEBUG_PATH}
  optimized ${DLIB_LIBRARY_OPTIMIZED_PATH})
add_custom_command(TARGET rover_tester POST_BUILD COMMAND rover_tester)
install(TARGETS rover_tester CONFIGURATIONS Debug
  DESTINATION ${TEST_INSTALL_DIRECTORY}/Debug)
install(TARGETS rover_tester CONFIGURATIONS Release RelWithDebInfo
  DESTINATION ${TEST_INSTALL_DIRECTORY}/Release)
