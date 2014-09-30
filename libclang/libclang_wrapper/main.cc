#include "CursorInfo.hpp"
#include "CursorCounter.hpp"
#include <llvm/Support/raw_ostream.h>

typedef unsigned LevelType;

const char *filename;
using namespace llvm;

CXChildVisitResult visitChildrenCallback(CXCursor cursor, CXCursor parent,
                                         CXClientData client_data) {
  CursorInfo cursorInfo = CursorInfo::getCursorInfo(cursor);
  cursorInfo.dump();
  LevelType level = *(LevelType *)client_data;
  errs() << '\n' << level << '\n';
  unsigned next = level + 1;
  clang_visitChildren(cursor, visitChildrenCallback, &next);

  return CXChildVisit_Continue;
}

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

  errs() << "clang version: " << getStrFromCXString(clang_getClangVersion())
         << '\n';

  LevelType level = 0;
  CXCursor cursor = clang_getTranslationUnitCursor(TU);
  clang_visitChildren(cursor, visitChildrenCallback, &level);

  clang_disposeTranslationUnit(TU);
  clang_disposeIndex(idx);

  return 0;
}
