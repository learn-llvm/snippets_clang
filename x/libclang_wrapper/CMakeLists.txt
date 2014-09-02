cmake_minimum_required(VERSION 2.8)
project(IRONY)

set (CMAKE_C_COMPILER             "clang")
set (CMAKE_CXX_COMPILER           "clang++")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR AND NOT MSVC_IDE)
  message(FATAL_ERROR "In-source builds are not allowed.")
endif()

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# libclang in non standard locations can cause some troubles.
option(USE_RPATH "Enable rpath for shared libraries (such as libclang.so)." OFF)

if (USE_RPATH)
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANGCXX)
  include(CheckCXXCompilerFlag)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Wall -Wshadow -std=c++11 -fno-rtti -fno-exceptions -O3")

  if (${CMAKE_GENERATOR} MATCHES "Ninja")
    check_cxx_compiler_flag("-fcolor-diagnostics" HAS_FCOLOR_DIAGNOSTICS_FLAG)
    if (HAS_FCOLOR_DIAGNOSTICS_FLAG)
      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcolor-diagnostics")
    endif()
  endif ()
endif ()

set(LLVM_SUFFIX "-3.4")
find_package(Llvm REQUIRED)

include_directories(${LLVM_INCLUDE_DIR})

add_definitions("-O3 -fno-rtti -fno-exceptions -Wextra -Wno-unused-parameter -std=c++11")
add_definitions("${LLVM_CFLAGS}")
if(UNIX)
  add_definitions("-DLLVM_ON_UNIX")
endif()
include(LLVMTools)
link_directories(${LLVM_LIBRARY_DIR})

set(BASE_LIBS pthread m dl ncurses)
set(LLVM_BASIC_LIBS LLVMIRReader LLVMAsmParser LLVMBitWriter LLVMLinker LLVMTransformUtils LLVMipa LLVMAnalysis LLVMTarget LLVMMC LLVMBitReader LLVMCore LLVMSupport)
set(CLANG_BASIC_LIBS clang clangFrontend clangFrontendTool clangDriver clangSerialization clangCodeGen clangParse clangSema clangEdit clangAnalysis clangAST clangLex clangBasic clangRewriteCore clangRewriteFrontend clangStaticAnalyzerFrontend clangStaticAnalyzerCheckers clangStaticAnalyzerCore clangARCMigrate)

file(GLOB TU *.cc)
add_executable(libclang_wrapper.out ${TU})
target_link_libraries(libclang_wrapper.out ${LLVM_BASIC_LIBS} ${CLANG_BASIC_LIBS} ${BASE_LIBS})
