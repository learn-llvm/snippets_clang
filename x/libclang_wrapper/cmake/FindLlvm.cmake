# Find the native LLVM includes and library
#
# LLVM_INCLUDE_DIR - where to find llvm include files
# LLVM_LIBRARY_DIR - where to find llvm libs
# LLVM_CFLAGS - llvm compiler flags
# LLVM_LDFLAGS - llvm linker flags
# LLVM_MODULE_LIBS - list of llvm libs for working with modules.
# LLVM_FOUND - True if llvm found.

find_program(LLVM_CONFIG_EXECUTABLE llvm-config${LLVM_SUFFIX})

if (NOT LLVM_CONFIG_EXECUTABLE)
  message(FATAL_ERROR "Could not find llvm-config")
endif (NOT LLVM_CONFIG_EXECUTABLE)

execute_process(
  COMMAND ${LLVM_CONFIG_EXECUTABLE} --includedir
  OUTPUT_VARIABLE LLVM_INCLUDE_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

execute_process(
  COMMAND ${LLVM_CONFIG_EXECUTABLE} --libdir
  OUTPUT_VARIABLE LLVM_LIBRARY_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

execute_process(
  COMMAND ${LLVM_CONFIG_EXECUTABLE} --cflags
  OUTPUT_VARIABLE LLVM_CFLAGS
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

execute_process(
  COMMAND ${LLVM_CONFIG_EXECUTABLE} --ldflags
  OUTPUT_VARIABLE LLVM_LDFLAGS
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

execute_process(
  COMMAND ${LLVM_CONFIG_EXECUTABLE} --version
  OUTPUT_VARIABLE LLVM_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )
STRING(REGEX MATCH "[0-9]+\\.[0-9]+" LLVM_VERSION ${LLVM_VERSION})

execute_process(
  COMMAND ${LLVM_CONFIG_EXECUTABLE} --bindir
  OUTPUT_VARIABLE LLVM_BINARY_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

include(Common)
replace_compiler_option(LLVM_CFLAGS "-DNDEBUG" "-UNDEBUG")
foreach(OPTLEVEL "-O" "-O0" "-O1" "O2" "-O3" "-O4" "-Os")
  replace_compiler_option(LLVM_CFLAGS ${OPTLEVEL} "")
endforeach()

message(STATUS "LLVM-config version: ${LLVM_VERSION}")
message(STATUS "LLVM lib dir: ${LLVM_LIBRARY_DIR}")
message(STATUS "LLVM bin dir: ${LLVM_BINARY_DIR}")
message(STATUS "LLVM compile flags: ${LLVM_CFLAGS}")
# message(STATUS "LLVM include dir: ${LLVM_INCLUDE_DIR}")
# message(STATUS "LLVM ldflags: ${LLVM_LDFLAGS}")
