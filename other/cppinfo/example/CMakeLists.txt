cmake_minimum_required(VERSION 2.8)
project(cpp_analyzer)

include_directories(${CMAKE_SOURCE_DIR}/libs)

add_executable(cpp_analyzer_example cpp_analyzer_example.cpp)

link_directories(${CMAKE_BINARY_DIR}/libs/)

target_link_libraries(cpp_analyzer_example cppinfo)

install(TARGETS cpp_analyzer_example DESTINATION bin)