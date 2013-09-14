#include <clang-c/Index.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char* argv[]) {
  CXIndex Index = clang_createIndex(0, 0);
  CXTranslationUnit TU = clang_parseTranslationUnit(
      Index, 0, (const char**)argv, argc, 0, 0, CXTranslationUnit_None);
  unsigned I = 0, J;
  unsigned N = clang_getNumDiagnostics(TU);

  for (I = 1; I != N; ++I) {
    CXDiagnostic diag = clang_getDiagnostic(TU, I);
    enum CXDiagnosticSeverity sev = clang_getDiagnosticSeverity(diag);
    switch (sev) {
      case 0:
        assert(0 && "impossible");
        break;
      case 1:
        fputs("[note] ", stderr);
        break;
      case 2:
        fputs("[warning] ", stderr);
        break;
      case 3:
        fputs("[error] ", stderr);
        break;
      case 4:
        fputs("[fatal error] ", stderr);
        break;
    }
    fputs(clang_getCString(clang_getDiagnosticSpelling(diag)), stderr);
    fputs("\n", stderr);
    unsigned line, column, offset;
    CXFile error_file;
    unsigned ranges;
    FILE* source;
    const char* err_filename = NULL;
    char* abs_filename = NULL;
    if ((ranges = clang_getDiagnosticNumRanges(diag)) >= 1) {
      CXSourceRange rangeset = clang_getDiagnosticRange(diag, 0);
      clang_getSpellingLocation(clang_getRangeStart(rangeset), &error_file,
                                &line, &column, &offset);
      err_filename = clang_getCString(clang_getFileName(error_file));
      fprintf(stderr, "[BEGIN] %s:%d:%d\n", err_filename, line, column);
      clang_getSpellingLocation(clang_getRangeEnd(rangeset), &error_file, &line,
                                &column, &offset);
      err_filename = clang_getCString(clang_getFileName(error_file));
      fprintf(stderr, "[END] %s:%d:%d\n", err_filename, line, column);
    } else {
      clang_getSpellingLocation(clang_getDiagnosticLocation(diag), &error_file,
                                &line, &column, &offset);
      err_filename = clang_getCString(clang_getFileName(error_file));
      abs_filename = realpath(err_filename, NULL);
      fprintf(stderr, "%s:%d:%d\n", abs_filename, line, column);
      free(abs_filename);
      source = fopen(err_filename, "r");
      int hold;
      fseek(source, offset - column + 1, SEEK_SET);
      while ((hold = getc(source)) != '\n') {
        putchar(hold);
      }
      printf("\n");
      for (J = 1; J < column; J++) {
        printf(" ");
      }
      printf("^\n\n");
      fclose(source);
    }
  }
  clang_disposeTranslationUnit(TU);
  clang_disposeIndex(Index);
  return 0;
}
