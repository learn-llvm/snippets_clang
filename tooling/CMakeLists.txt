set(CLANG_LIBS
  -lclangASTMatchers
  -lclangDynamicASTMatchers
  -lclangTooling
  -lclangFrontendTool
  -lclangFrontend
  -lclangDriver
  -lclangSerialization
  -lclangCodeGen
  -lclangParse
  -lclangSema
  -lclangStaticAnalyzerFrontend
  -lclangStaticAnalyzerCheckers
  -lclangStaticAnalyzerCore
  -lclangARCMigrate
  -lclangRewriteFrontend
  -lclangRewrite
  -lclangAnalysis
  -lclangEdit
  -lclangAST
  -lclangLex
  -lclangBasic
  )

set(CLANG_TOOLS
  # ast
  # complex
  # clang_check
  cfe
  # rewrite1
  # rewrite2
  # rewrite3
  # tokens
  # toplevel_decls
  # try_matcher
  )
foreach(TOOL ${CLANG_TOOLS})
  add_subdirectory(${TOOL})
endforeach()
