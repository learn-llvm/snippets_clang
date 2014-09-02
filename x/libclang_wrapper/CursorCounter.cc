#include "CursorCounter.hpp"
#include "llvm/Support/raw_ostream.h"

extern char const *filename;

CXChildVisitResult statsVisitor(CXCursor cursor, CXCursor parent,
                                CXClientData client_data) {
  CursorCounter &counter = CursorCounter::getInstance();
  std::string file = CursorCounter::getCursorFileName(cursor);
  if (file == filename) {
    counter.insert(cursor.kind);
  }
  return CXChildVisit_Recurse;
}

bool CursorCounter::insert(CXCursorKind kind) {
  auto search = cursor2counter_.find(kind);
  if (search != cursor2counter_.end()) {
    ++search->second;
    return true;
  }
  cursor2counter_.insert(std::make_pair(kind, 1));
  return false;
}

std::string CursorCounter::getCursorFileName(CXCursor cursor) {
  CXFile file;
  clang_getSpellingLocation(clang_getCursorLocation(cursor), &file, nullptr,
                            nullptr, nullptr);
  CXString fileCXStr = clang_getFileName(file);
  return getStrFromCXString(fileCXStr);
}

void CursorCounter::visit(CXTranslationUnit const &TU) {
  clang_visitChildren(clang_getTranslationUnitCursor(TU), statsVisitor,
                      nullptr);
}

void CursorCounter::dump() {
  for (auto const &ele : cursor2counter_) {
    CXString cursorCXStr = clang_getCursorKindSpelling(ele.first);
    llvm::errs() << getStrFromCXString(cursorCXStr) << ": " << ele.second
                 << "\n";
  }
}
