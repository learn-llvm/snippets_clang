#include <cstdio>
#include <string>

#include "llvm/Support/Signals.h"
#include "Common.hpp"

using namespace llvm;

typedef struct {
  std::string name;
} MatchPattern;

enum CXChildVisitResult VisitorCallBack(CXCursor cursor, CXCursor parent,
                                        CXClientData clientData) {
  CXFile file;
  unsigned line, column, offset;
  auto mp = static_cast<MatchPattern*>(clientData);
  clang_getInstantiationLocation(clang_getCursorLocation(cursor), &file, &line,
                                 &column, &offset);
  if (file == nullptr) {
    return CXChildVisit_Recurse;
  }
  auto info = getStrFromCXString(clang_getCursorSpelling(cursor));
  if (mp->name == info) {
    auto kind = clang_getCursorKind(cursor);
    errs() << format("%s: (%u, %u) %s: %s\n",
                     getStrFromCXString(clang_getFileName(file)).data(), line,
                     column, getStrFromCXString(
                                 clang_getCursorKindSpelling(kind)).data(),
                     info.data());
  }
  return CXChildVisit_Recurse;
}

constexpr int ARGC = 3;

int main(int argc, char* argv[]) {
  sys::PrintStackTraceOnErrorSignal();
  if (argc < ARGC) {
    errs() << "usage: " << argv[0] << " filename pattern\n";
    std::exit(1);
  }

  char* file = argv[1];
  char* symbol = argv[2];

  auto index = clang_createIndex(0, 0);
  auto m = MatchPattern{symbol};

  auto tu =
      clang_parseTranslationUnit(index, file, argv + ARGC, argc - ARGC, 0u, 0u,
                                 CXTranslationUnit_DetailedPreprocessingRecord);

  CXCursor tu_cur = clang_getTranslationUnitCursor(tu);
  clang_visitChildren(tu_cur, VisitorCallBack, &m);
}
