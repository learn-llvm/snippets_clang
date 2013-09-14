cmake_minimum_required (VERSION 2.8)

# set(CMAKE_DISABLE_SOURCE_CHANGES ON)
# set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set (CMAKE_C_COMPILER "clang")
set (CMAKE_CXX_COMPILER "clang++")
project (my_plugin)

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )

set (LLVM_SRC_DIR /usr/lib/llvm-3.4/include)
set (LLVM_BUILD_DIR /usr/lib/llvm-3.4/lib)

add_definitions (-D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS)
add_definitions (-D_GNU_SOURCE -DHAVE_CLANG_CONFIG_H)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -fno-common -Woverloaded-virtual -Wcast-qual -fno-strict-aliasing -pedantic -Wno-long-long -Wall -W -Wno-unused-parameter -Wwrite-strings -fexceptions -frtti -std=c++11")
set (CMAKE_MODULE_LINKER_FLAGS "-Wl,-flat_namespace -Wl,-undefined -Wl,suppress")

set (LLVM_LIBS LLVMJIT LLVMX86CodeGen LLVMX86AsmParser LLVMX86Disassembler LLVMExecutionEngine LLVMAsmPrinter LLVMSelectionDAG LLVMX86AsmPrinter LLVMX86Info LLVMMCParser LLVMCodeGen LLVMX86Utils LLVMScalarOpts LLVMInstCombine LLVMTransformUtils LLVMipa LLVMAnalysis LLVMTarget LLVMCore LLVMMC LLVMSupport)

macro(add_clang_plugin name)
  set (srcs ${ARGN})

  include_directories( "${LLVM_SRC_DIR}")
  link_directories( "${LLVM_BUILD_DIR}")

  add_library( ${name} SHARED ${srcs} )

  if (SYMBOL_FILE)
    set_target_properties( ${name} PROPERTIES LINK_FlAGS
      "-exported_symbols_list ${SYMBOL_FILE}")
  endif()

  foreach (clang_lib ${CLANG_LIBS})
    target_link_libraries( ${name} ${clang_lib} )
  endforeach()

  foreach (llvm_lib ${LLVM_LIBS})
    target_link_libraries( ${name} ${llvm_lib} )
  endforeach()

  foreach (user_lib ${USER_LIBS})
    target_link_libraries( ${name} ${user_lib} )
  endforeach()

endmacro(add_clang_plugin)

add_subdirectory(src)
add_subdirectory(test)