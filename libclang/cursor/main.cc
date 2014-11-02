#include <cstdlib>
#include <cassert>
#include <cstddef>
#include <string>

#include "Common.hpp"

constexpr int ARG_NUM = 4;

void checkCursor(CXCursor cursor) {
  auto kind = clang_getCursorKind(cursor);
  if (clang_isInvalid(kind)) {
    errs() << "invalid cursor kind: "
           << getStrFromCXString(clang_getCursorKindSpelling(kind)) << '\n';
    std::exit(1);
  }
}

static void dumpCursorLoc(CXCursor const& cursor, char const* hint) {
  auto dumpRealLoc = [](CXSourceLocation& loc, auto hint, auto& generator) {
    CXFile file;
    unsigned line, col;
    generator(loc, &file, &line, &col, nullptr);
    assert(file != nullptr);
    auto fileNameStr = getStrFromCXString(clang_getFileName(file));
    errs() << "[" << hint << "] " << fileNameStr << ":" << line << ":" << col
           << '\n';
  };
  auto location = clang_getCursorLocation(cursor);
  auto range = clang_getCursorExtent(cursor);
  // FIXME incorrect for several macros
  errs() << hint << "\t(" << location.int_data << ", " << range.begin_int_data
         << "-" << range.end_int_data << ")\n";
  dumpRealLoc(location, "spellingLoc", clang_getSpellingLocation);
  dumpRealLoc(location, "ExpansionLoc", clang_getExpansionLocation);
  {
    CXString fileCXStr;
    unsigned line, col;
    clang_getPresumedLocation(location, &fileCXStr, &line, &col);
    errs() << "[presumedLoc] " << getStrFromCXString(fileCXStr) << ", " << line
           << ":" << col << '\n';
  }
}

void dumpCursorInfo(CXCursor const& cursor) {
  if (clang_Cursor_isNull(cursor)) return;
  checkCursor(cursor);
  dumpCursorLoc(cursor, "cursor");
  auto canonicalCursor = clang_getCanonicalCursor(cursor);
  auto equalCursor = clang_equalCursors(cursor, canonicalCursor);
  if (equalCursor == 0) {  //==0 means different
    checkCursor(canonicalCursor);
    dumpCursorLoc(canonicalCursor, "canonicalCursor");
  }
  auto refCursor = clang_getCursorReferenced(cursor);
  checkCursor(refCursor);
  dumpCursorLoc(refCursor, "refCursor");
}

int main(int argc, char* argv[]) {
  if (argc < ARG_NUM) {
    errs() << "usage: " << argv[0] << " filename line column\n";
    std::exit(1);
  }

  auto const* filename = argv[1];

  auto index = clang_createIndex(0, 0);
  auto tu = clang_parseTranslationUnit(
      index, filename,  // source_filename, 0 if included in command_line_args
      argv + ARG_NUM,   // command_line_args
      argc - ARG_NUM,   // num_command_line_args
      0u,               // unsaved_files
      0u,               // num_unsaved_files
      CXTranslationUnit_PrecompiledPreamble |
          CXTranslationUnit_DetailedPreprocessingRecord |
          CXTranslationUnit_CacheCompletionResults);
  auto file = clang_getFile(tu, filename);

  auto const line = atoi(argv[2]);
  auto const column = atoi(argv[3]);
  auto location = clang_getLocation(tu, file, line, column);
  auto cursor = clang_getCursor(tu, location);
  dumpCursorInfo(cursor);
  clang_disposeTranslationUnit(tu);
  clang_disposeIndex(index);

  return 0;
}
