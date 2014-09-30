set(LIBCLANG_TOOLS
  cc_lookup
  clang_test_it
  clang_visit
  CodeComplete
  compilation_db
  diagnose
  diagnosis
  func_sig
  inclusion_visitor
  libclang_wrapper
  symbol_scanner
  token
  visit_test
  )

list(APPEND FULL_LLVM_LIBS -lclang)

foreach( TOOL ${LIBCLANG_TOOLS})
  add_subdirectory(${TOOL})
endforeach()