#include "CursorInfo.hpp"
#include "CursorCounter.hpp"

#include <cstdlib>

typedef unsigned LevelType;

using namespace llvm;

struct ClientInfo {
  LevelType level;
  std::string filename;
};

enum CXChildVisitResult visitChildrenCallback(CXCursor cursor, CXCursor parent,
                                              CXClientData client_data) {
  auto *info = static_cast<ClientInfo *>(client_data);
  ///
  CXFile file;
  clang_getInstantiationLocation(clang_getCursorLocation(cursor), &file,
                                 nullptr, nullptr, nullptr);
  if (file == nullptr) {
    return CXChildVisit_Continue;
  }
  std::string filename(getStrFromCXString(clang_getFileName(file)));
  std::string const &wantFilename = info->filename;
  if (filename != wantFilename) {
    return CXChildVisit_Continue;
  }
  ///
  errs() << '\n' << info->level++ << '\n';
  CursorInfo cursorInfo = CursorInfo::getCursorInfo(cursor);
  cursorInfo.dump();
  /// clang_visitChildren(cursor, visitChildrenCallback, &info);
  return CXChildVisit_Continue;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    std::fprintf(stderr, "usage: %s ${c_cxx_src_file}", argv[0]);
    exit(1);
  }
  char const *filename = argv[1];
  CXIndex idx = clang_createIndex(1, 0);
  CXTranslationUnit TU = clang_parseTranslationUnit(
      idx, filename, argv + 2, argc - 2, 0, 0,
      CXTranslationUnit_PrecompiledPreamble |
          CXTranslationUnit_DetailedPreprocessingRecord);
  clang_reparseTranslationUnit(TU, 0, nullptr, 0);

  errs() << "clang version: " << getStrFromCXString(clang_getClangVersion())
         << '\n';

  auto cursor = clang_getTranslationUnitCursor(TU);
  auto cursorInfo = CursorInfo::getCursorInfo(cursor);
  cursorInfo.dump();
  errs() << "\n\n";
  /// auto module = clang_Cursor_getModule(cursor);
  /// errs() << getStrFromCXString(clang_Module_getName(module));
  ClientInfo info{0, filename};
  clang_visitChildren(cursor, visitChildrenCallback, &info);
  /// clang_visitChildren(cursor, visitChildrenCallback, nullptr);

  clang_disposeTranslationUnit(TU);
  clang_disposeIndex(idx);

  return 0;
}
