#include <clang-c/Index.h>
#include <stdlib.h>
#include <stdio.h>

static void printString(CXString string) {
  const char *cstr = clang_getCString(string);
  if (cstr && *cstr) {
    printf("%s ", cstr);
  }
  clang_disposeString(string);
}

static void printCursor(CXCursor cursor) {
  CXFile file;
  unsigned off, line, col;
  CXSourceLocation location = clang_getCursorLocation(cursor);
  printString(clang_getCursorDisplayName(cursor));
  printString(clang_getCursorKindSpelling(clang_getCursorKind(cursor)));
  clang_getSpellingLocation(location, &file, &line, &col, &off);

  CXType type = clang_getCursorType(cursor);
  CXString str = clang_getTypeKindSpelling(type.kind);
  printString(str);

  // CXString cx_filename = clang_getFileName(file);
  // if (cx_filename.data) {
  //   puts("");
  //   printString(cx_filename);
  //   CXSourceRange range = clang_getCursorExtent(cursor);
  //   unsigned start, end;
  //   clang_getSpellingLocation(clang_getRangeStart(range), 0, 0, 0, &start);
  //   clang_getSpellingLocation(clang_getRangeEnd(range), 0, 0, 0, &end);
  //   printf("[%d:%d] (%d-%d) ", line, col, start, end);
  // }
  printf("\n");
}

static enum CXChildVisitResult visit_func(CXCursor cursor, CXCursor parent,
                                          CXClientData userData) {
  (void)parent;
  int indent = *(int *)userData;
  int i;
  for (i = 0; i < indent; ++i) {
    printf("~");
  }
  printCursor(cursor);
  CXCursor ref = clang_getCursorReferenced(cursor);
  if (!clang_isInvalid(clang_getCursorKind(ref)) &&
      !clang_equalCursors(ref, cursor)) {
    for (i = 0; i < indent; ++i) {
      printf("*");
    }
    printf("-> ");
    printCursor(ref);
  }

  ++indent;
  clang_visitChildren(cursor, visit_func, &indent);
  return CXChildVisit_Continue;
}

int main(int argc, char **argv) {
  if (argc < 2) return 1;
  CXIndex index = clang_createIndex(0, 0);
  const char *const *args = 0;
  if (argc > 2) args = (const char * const *)&argv[2];

  CXTranslationUnit unit =
      clang_parseTranslationUnit(index, argv[1], args, argc - 2, 0, 0,
                                 clang_defaultEditingTranslationUnitOptions());
  int indent = 0;
  clang_visitChildren(clang_getTranslationUnitCursor(unit), visit_func,
                      &indent);

  clang_disposeTranslationUnit(unit);
  clang_disposeIndex(index);
  return 0;
}
