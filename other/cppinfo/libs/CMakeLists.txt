cmake_minimum_required(VERSION 2.8)
project(CppInfo)

add_library(cppinfo SHARED cpp_analyzer.cpp cpp_info.cpp)

target_link_libraries(cppinfo clang)

install(TARGETS cppinfo DESTINATION lib)
install(FILES basic_type_info.h cpp_analyzer.h cpp_info.h DESTINATION include/cppinfo)