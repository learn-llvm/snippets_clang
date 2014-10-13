set(LIBCLANG_TOOLS
  cursor
  match
  ccomplete
  diagnosis
  wrapper
  voidcaster
  )

list(APPEND FULL_LLVM_LIBS -lclang)

foreach( TOOL ${LIBCLANG_TOOLS})
  add_subdirectory(${TOOL})
endforeach()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

project(Common)
add_library(${PROJECT_NAME} STATIC
  Common.cc
  )
target_link_libraries(${PROJECT_NAME} ${CLANG_LIBS} ${FULL_LLVM_LIBS})