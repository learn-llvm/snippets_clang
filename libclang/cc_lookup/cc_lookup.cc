#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <clang-c/Index.h>

const int ARG_NUM = 4;  /// *test

static inline void printCXString(CXString cx_str) {
  const char* c_str = clang_getCString(cx_str);
  if (c_str) {
    printf("%s", c_str);
  }
  clang_disposeString(cx_str);
}

void getRef(CXTranslationUnit TU, CXSourceLocation location) {
  CXCursor cursor = clang_getCursor(TU, location);
  // TODO do we need to dispose kind?
  enum CXCursorKind kind = clang_getCursorKind(cursor);

  if (clang_isInvalid(kind)) {
    CXString cx_kind = clang_getCursorKindSpelling(kind);
    fprintf(stderr, "[error] cursor kind is: ");
    printCXString(cx_kind);
    return;
  }

  CXCursor decl_cursor = clang_getCursorReferenced(cursor);
  // CXCursor definition = clang_getCursorDefinition(cursor);
  if (clang_isInvalid(decl_cursor.kind)) {
    CXString cx_kind = clang_getCursorKindSpelling(kind);
    fprintf(stderr,
            "[error] cannot find the declaration for cursor whose kind is:");
    printCXString(cx_kind);
    return;
  }
  CXSourceLocation def_loc = clang_getCursorLocation(decl_cursor);
  assert(&def_loc);

  CXFile def_file;
  unsigned def_line = 0, def_column = 0, def_offset = 0;
  clang_getSpellingLocation(def_loc, &def_file, &def_line, &def_column,
                            &def_offset);
  if (!def_file) {
    fprintf(stderr, "[error] no def file found");
    return;
  }
  CXString def_file_name_cx = clang_getFileName(def_file);
  const char* def_file_name = def_file_name =
      clang_getCString(def_file_name_cx);
  CXString name_cx = clang_getCursorSpelling(cursor);
  printf("%s\t%d\t%d\t%s", def_file_name, def_line, def_column,
         clang_getCString(name_cx));

  clang_disposeString(name_cx);
  clang_disposeString(def_file_name_cx);
}

/**
 * \brief print whether the property is present
 *
 * @param name the property name
 * @param hasProp the boolean
 */

static void printCursor(CXTranslationUnit TU, CXSourceLocation location) {
  CXFile file;
  unsigned off, line, col;
  CXCursor cursor = clang_getCursor(TU, location);
  clang_getSpellingLocation(location, &file, &line, &col, &off);
  CXString fileName = clang_getFileName(file);
  const char* fileNameCStr = clang_getCString(fileName);
  if (fileNameCStr) {
    CXSourceRange range = clang_getCursorExtent(cursor);
    unsigned start, end;
    clang_getSpellingLocation(clang_getRangeStart(range), 0, 0, 0, &start);
    clang_getSpellingLocation(clang_getRangeEnd(range), 0, 0, 0, &end);
    printf("%s:%d:%d\n(%d, %d-%d) ", fileNameCStr, line, col, off, start, end);
  }
  CXCursor realCursor = clang_getCanonicalCursor(cursor);
  clang_disposeString(fileName);
  printf("\n[doc]");
  printCXString(clang_Cursor_getBriefCommentText(cursor));
  printf("\n[rawdoc]");
  printCXString(clang_Cursor_getRawCommentText(realCursor));
  CXComment comment = clang_Cursor_getParsedComment(cursor);
  printf("\n[html]\n");
  CXString html_comment = clang_FullComment_getAsHTML(comment);
  printCXString(html_comment);
  printf("\n[xml]\n");
  CXString xml_comment = clang_FullComment_getAsXML(comment);
  printCXString(xml_comment);
  printf("\nisVirtualCall=");
  printf("%d", clang_Cursor_isDynamicCall(cursor));
  printf("\n[Module]");
  CXModule module = clang_Cursor_getModule(cursor);
  printCXString(clang_Module_getName(module));
  printf("\nisDefinition=");
  printf("%d\n", clang_isCursorDefinition(cursor));
}

// TODO clang_getDiagnosticNumFixIts, clang_getDiagnosticFixIt
static int diagnoseTU(CXTranslationUnit TU) {
  int has_error = 0;
  unsigned n = clang_getNumDiagnostics(TU);
  // the first Diagnostics is "link" warning
  for (unsigned i = 1; i != n; ++i) {
    CXDiagnostic diag = clang_getDiagnostic(TU, i);
    CXString msg = clang_formatDiagnostic(
        diag, CXDiagnostic_DisplaySourceLocation | CXDiagnostic_DisplayColumn |
                  CXDiagnostic_DisplaySourceRanges |
                  CXDiagnostic_DisplayOption |
                  CXDiagnostic_DisplayCategoryName);
    enum CXDiagnosticSeverity sev = clang_getDiagnosticSeverity(diag);
    has_error = (sev >= CXDiagnostic_Error);
    if (has_error) {
      fprintf(stderr, "%s\n", clang_getCString(msg));
    }
    clang_disposeString(msg);
    clang_disposeDiagnostic(diag);
  }
  return has_error;
}

int main(int argc, char* argv[]) {
  printf("ARG_NUM:%d\n", ARG_NUM);

  assert(argc >= ARG_NUM + 1 && "too few arguments");

  const char* org_file_name = argv[1];
  const int line = atoi(argv[2]);
  const int column = atoi(argv[3]);
  const int options = atoi(argv[4]);
  for (int i = 0; i < argc; ++i) {
    printf("[%d] %s\n", i, argv[i]);
  }

  CXIndex index = clang_createIndex(0, 0);
  const char* const* clang_args = (const char * const*)(argv + ARG_NUM);
  CXTranslationUnit TU = clang_parseTranslationUnit(
      index,
      org_file_name,   // source_filename, 0 if included in command_line_args
      clang_args,      // command_line_args
      argc - ARG_NUM,  // num_command_line_args
      0,               // unsaved_files
      0,               // num_unsaved_files
      CXTranslationUnit_PrecompiledPreamble |
          CXTranslationUnit_DetailedPreprocessingRecord |
          CXTranslationUnit_CacheCompletionResults);
  if (options == 4) {
    diagnoseTU(TU);
  } else {
    CXFile cx_file = clang_getFile(TU, org_file_name);
    CXSourceLocation location = clang_getLocation(TU, cx_file, line, column);
    if (options == 2) {
      printCursor(TU, location);
    } else if (options == 1) {
      getRef(TU, location);
    }
  }
  clang_disposeTranslationUnit(TU);
  clang_disposeIndex(index);

  return 0;
}
