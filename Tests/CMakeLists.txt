include_directories(./../Dependencies/Catch2-2.6.1/single_include/catch2)
file(GLOB source_files ./Source/*.cpp)
add_executable(rover_tester ${source_files})
add_custom_command(TARGET rover_tester POST_BUILD COMMAND rover_tester)
