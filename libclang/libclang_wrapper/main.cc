#include "CursorInfo.hpp"
#include "CursorCounter.hpp"
#include <llvm/Support/raw_ostream.h>

const char *filename;

using namespace llvm;

int main(int argc, char **argv) {
  if (argc < 2) {
    std::fprintf(stderr, "usage: %s ${c_cxx_src_file}", argv[0]);
    std::exit(1);
  }
  filename = argv[1];
  CXIndex idx = clang_createIndex(1, 0);
  CXTranslationUnit TU = clang_parseTranslationUnit(
      idx, filename, argv + 2, argc - 2, 0, 0,
      CXTranslationUnit_PrecompiledPreamble |
          CXTranslationUnit_DetailedPreprocessingRecord);
  clang_reparseTranslationUnit(TU, 0, nullptr, 0);
  CursorCounter &counter = CursorCounter::getInstance();
  counter.visit(TU);
  counter.dump();
  return 0;
}
