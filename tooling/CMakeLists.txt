# vim: set ft=cmake ts=2 sw=0 tw=2:

cmake_minimum_required(VERSION 3.25)

project(my-libtooling)

set(CMAKE_CXX_STANDARD 17)
set (CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

if(NOT LLVM_ENABLE_RTTI)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()
if(NOT LLVM_ENABLE_EH)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
endif()
if(LLVM_ENABLE_UNWIND_TABLES)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -funwind-tables")
endif()

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
find_package(Clang REQUIRED CONFIG)
message(STATUS "Using ClangConfig.cmake")

message(STATUS "LLVM includes: ${LLVM_INCLUDE_DIRS}")
message(STATUS "Clang includes: ${CLANG_INCLUDE_DIRS}")
include_directories(${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})

message(STATUS "LLVM defs: ${LLVM_DEFINITIONS_LIST}")
add_definitions(${LLVM_DEFINITIONS_LIST})

set(CLANG_LIBS
  clangAST
  clangASTMatchers
  clangBasic
  clangDriver
  clangFormat
  clangFrontend
  clangIndex
  clangLex
  clangRewrite
  clangSema
  clangSerialization
  clangTooling
  clangToolingCore
  clangToolingInclusions
  clangToolingInclusionsStdlib
  clangToolingSyntax
)

llvm_map_components_to_libnames(llvm_libs support core targetparser)

set(CLANG_TOOLS
  ast
  cfe
  decls
)
foreach(tool ${CLANG_TOOLS})
  add_subdirectory(${tool})
endforeach()
