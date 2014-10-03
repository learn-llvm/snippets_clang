#include "Common.hpp"
#include <map>

auto printLoc = [](char const *hint, CXSourceLocation const &loc) {
  CXFile file;
  unsigned line, column;
  clang_getSpellingLocation(loc, &file, &line, &column, nullptr);
  errs() << hint << ": " << getStrFromCXString(clang_getFileName(file)) << ":"
         << line << ":" << column << '\n';
};

void diagnosis(const CXTranslationUnit &tu) {
  auto diagSet = clang_getDiagnosticSetFromTU(tu);
  auto numDiag = clang_getNumDiagnosticsInSet(diagSet);
  errs() << format("numDiag: %d\n\n", numDiag);

  for (auto i = 0u; i < numDiag; i++) {
    auto diag = clang_getDiagnosticInSet(diagSet, i);
    errs() << "Diagnosis[" << i << "]\n";
    {  // diagosis msg
      static auto formatOption =
          CXDiagnostic_DisplaySourceLocation | CXDiagnostic_DisplayColumn |
          CXDiagnostic_DisplaySourceRanges | CXDiagnostic_DisplayOption |
          CXDiagnostic_DisplayCategoryId | CXDiagnostic_DisplayCategoryName;
      errs() << "msg: " << getStrFromCXString(clang_formatDiagnostic(
                               diag, formatOption)) << '\n';

      auto childDiagSet = clang_getChildDiagnostics(diag);
      auto numChildDiag = clang_getNumDiagnosticsInSet(childDiagSet);
      errs() << "NumChildDiag: " << numChildDiag << '\n';
      // TODO: show child DiagnosticSet recursively(?)
    }
    {  // severity
      typedef std::map<CXDiagnosticSeverity, char const *> SeverityMap;
#define PAIR(name) \
  { CXDiagnostic_##name, #name }
      static SeverityMap severityMap = {
          PAIR(Ignored), PAIR(Note), PAIR(Warning), PAIR(Error), PAIR(Fatal)};
#undef PAIR
      auto severity = clang_getDiagnosticSeverity(diag);
      errs() << severityMap[severity] << '\n';
    }
    printLoc("Location", clang_getDiagnosticLocation(diag));
    errs() << "Category: "
           << getStrFromCXString(clang_getDiagnosticCategoryText(diag)) << '\n';
    {  // range
      auto numRange = clang_getDiagnosticNumRanges(diag);
      errs() << "numRange: " << numRange << '\n';
      for (auto j = 0u; j < numRange; j++) {
        auto range = clang_getDiagnosticRange(diag, j);
        printLoc("start", clang_getRangeStart(range));
        printLoc("end", clang_getRangeEnd(range));
      }
    }
    {  // fixit
      auto numFixit = clang_getDiagnosticNumFixIts(diag);
      errs() << "NumFixit: " << numFixit << '\n';
      for (auto j = 0u; j < numFixit; j++) {
        auto fixit = clang_getDiagnosticFixIt(diag, j, nullptr);
        errs() << "Fixit[" << j << "]:" << getStrFromCXString(fixit);
      }
    }
    errs() << '\n';
    clang_disposeDiagnostic(diag);
  }
  clang_disposeDiagnosticSet(diagSet);
}

constexpr int NUM = 2;
int main(int argc, char **argv) {
  if (argc < NUM) {
    errs() << "usage: " << argv[0] << " filename [options ...]\n";
    std::exit(1);
  }

  CXIndex index = clang_createIndex(1, 0);
  auto const filename = argv[1];
  auto tu = clang_parseTranslationUnit(index, filename, argv + NUM, argc - NUM,
                                       nullptr, 0u, 0u);
  if (tu == nullptr) {
    printf("Cannot parse translation unit\n");
    return 1;
  }

  diagnosis(tu);

  clang_disposeTranslationUnit(tu);
  clang_disposeIndex(index);
  return 0;
}
