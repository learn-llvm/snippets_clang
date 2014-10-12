cmake_minimum_required(VERSION 2.8)

link_directories(/usr/lib/llvm/)
set(CMAKE_CXX_FLAGS "-DDEBUG -Wall -std=c++11 -O0 -g ${CMAKE_CXX_FLAGS}")

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

add_subdirectory(libs)
add_subdirectory(example)