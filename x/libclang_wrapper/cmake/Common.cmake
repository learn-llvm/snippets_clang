function(replace_compiler_option var old new)
  # Replaces a compiler option or switch `old' in `var' by `new'.
  # If `old' is not in `var', appends `new' to `var'.
  # Example: replace_compiler_option(CMAKE_CXX_FLAGS_RELEASE "-O3" "-O2")
  # If the option already is on the variable, don't add it:
  if( "${${var}}" MATCHES "(^| )${new}($| )" )
    set(n "")
  else()
    set(n "${new}")
  endif()
  if( "${${var}}" MATCHES "(^| )${old}($| )" )
    string( REGEX REPLACE "(^| )${old}($| )" " ${n} " ${var} "${${var}}" )
  else()
    set( ${var} "${${var}} ${n}" )
  endif()
  set( ${var} "${${var}}" PARENT_SCOPE )
endfunction(replace_compiler_option)