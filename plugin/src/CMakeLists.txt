set (CLANG_LIBS clang clangFrontend clangFrontendTool clangDriver clangSerialization clangCodeGen clangParse clangSema clangEdit clangAnalysis clangAST clangLex clangBasic clangRewriteCore clangRewriteFrontend clangStaticAnalyzerFrontend clangStaticAnalyzerCheckers clangStaticAnalyzerCore clangARCMigrate clangTooling clangASTMatchers)

set (USER_LIBS pthread)

file(GLOB SIMPLE_PLUGINS *.cpp)
add_clang_plugin(plugin
  ${SIMPLE_PLUGINS}
  )