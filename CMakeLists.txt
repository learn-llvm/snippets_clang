cmake_minimum_required(VERSION 2.8)

PROJECT(LLVMLearn)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

set (CMAKE_EXPORT_COMPILE_COMMANDS 1)
add_definitions("-O3 -fno-rtti -fno-exceptions -Wextra -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS "-std=c++1y -fcolor-diagnostics")

find_package(Llvm REQUIRED)
add_definitions("${LLVM_CFLAGS}")

include_directories(${LLVM_SRC_ROOT}/tools/clang/include)
include_directories(${LLVM_OBJ_ROOT}/tools/clang/include)

set(FULL_LLVM_LIBS ${LLVM_SYSTEM_LIBS} ${LLVM_MODULE_LIBS})

link_directories(${LLVM_LIBRARY_DIR})

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# set(CMAKE_VERBOSE_MAKEFILE 1)

# -------------------------------------------------------------------
add_subdirectory(tooling)
