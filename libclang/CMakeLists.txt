set(LIBCLANG_TOOLS
  cc_lookup
  clang_test_it
  match_symbol
  CodeComplete
  diagnosis
  func_sig
  libclang_wrapper
  token
  )

list(APPEND FULL_LLVM_LIBS -lclang)

foreach( TOOL ${LIBCLANG_TOOLS})
  add_subdirectory(${TOOL})
endforeach()