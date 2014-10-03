#include <cstdio>
#include <cstdlib>
#include <string>
#include <map>

#include "Common.hpp"

typedef std::map<CXCompletionChunkKind, std::string> CompletionMap;
#define PAIR(name) \
  { CXCompletionChunk_##name, #name }
CompletionMap completionMap{
    PAIR(Optional),        PAIR(TypedText),    PAIR(Text),
    PAIR(Placeholder),     PAIR(Informative),  PAIR(CurrentParameter),
    PAIR(LeftParen),       PAIR(RightParen),   PAIR(LeftBracket),
    PAIR(RightBracket),    PAIR(LeftBrace),    PAIR(RightBrace),
    PAIR(LeftAngle),       PAIR(RightAngle),   PAIR(Comma),
    PAIR(ResultType),      PAIR(Colon),        PAIR(Equal),
    PAIR(HorizontalSpace), PAIR(VerticalSpace)};
#undef PAIR

typedef std::map<CXAvailabilityKind, char const *> AvailMap;
#define PAIR(name) \
  { CXAvailability_##name, #name }
AvailMap availMap{PAIR(Available),    PAIR(Deprecated),
                  PAIR(NotAvailable), PAIR(NotAccessible)};
#undef PAIR

constexpr int NUM = 4;
int main(int argc, char **argv) {
  if (argc < NUM) {
    errs() << "usage: " << argv[0] << " filename line column [options ...]\n";
    exit(1);
  }

  auto const filename = argv[1];
  auto lineno = atoi(argv[2]);
  auto columnno = atoi(argv[3]);

  auto index = clang_createIndex(1, 0);

  auto tu =
      clang_parseTranslationUnit(index, filename, argv + 4, argc - 4, nullptr,
                                 0, CXTranslationUnit_Incomplete);
  if (tu == nullptr) {
    errs() << ("Cannot parse translation unit\n");
    std::exit(1);
  }

  auto *compResults =
      clang_codeCompleteAt(tu, filename, lineno, columnno, nullptr, 0u,
                           clang_defaultCodeCompleteOptions());
  if (compResults == nullptr) {
    errs() << "no compResults\n";
    std::exit(1);
  }

  {
    unsigned isIncomplete;
    auto kind = clang_codeCompleteGetContainerKind(compResults, &isIncomplete);
    errs() << "InComplete: " << isIncomplete << '\n';
    errs() << "Kind: " << getStrFromCXString(clang_getCursorKindSpelling(kind))
           << '\n';

    errs() << "USR: " << getStrFromCXString(
                             clang_codeCompleteGetContainerUSR(compResults));

    errs() << "Context: " << clang_codeCompleteGetContexts(compResults)
           << "\n\n";

    for (auto i = 0u; i < compResults->NumResults; i++) {
      errs() << "RESULT: " << i << '\n';
      auto &result = compResults->Results[i];
      auto &compString = result.CompletionString;
      auto kind = result.CursorKind;
      errs() << "kind: "
             << getStrFromCXString(clang_getCursorKindSpelling(kind)) << '\n';

      auto availavility = clang_getCompletionAvailability(compString);
      auto availavilityText = availMap[availavility];
      errs() << "Availavility: " << availavilityText << '\n';

      auto priority = clang_getCompletionPriority(compString);
      errs() << "Priority: " << priority << '\n';

      errs() << "Comment: "
             << getStrFromCXString(clang_getCompletionBriefComment(compString))
             << '\n';

      auto numChunks = clang_getNumCompletionChunks(compString);
      for (auto j = 0u; j < numChunks; j++) {
        errs() << "chunk[" << j << "] Kind: "
               << completionMap[clang_getCompletionChunkKind(compString, j)]
               << " Text: " << getStrFromCXString(clang_getCompletionChunkText(
                                   compString, j)) << '\n';
        /// auto child = clang_getCompletionChunkCompletionString(compString,
        /// j);
      }

      auto numAnnotations = clang_getCompletionNumAnnotations(compString);
      for (auto j = 0u; j < numAnnotations; j++) {
        errs() << "Annotation[" << j
               << "]=" << getStrFromCXString(
                              clang_getCompletionAnnotation(compString, j));
      }
      errs() << "\n";
    }
  }

  {
    auto diagNum = clang_codeCompleteGetNumDiagnostics(compResults);
    for (auto i = 0u; i < diagNum; i++) {
      clang_codeCompleteGetDiagnostic(compResults, i);
      auto diag = clang_getDiagnostic(tu, i);
      static auto formatOption =
          CXDiagnostic_DisplaySourceLocation | CXDiagnostic_DisplayColumn |
          CXDiagnostic_DisplaySourceRanges | CXDiagnostic_DisplayOption |
          CXDiagnostic_DisplayCategoryId | CXDiagnostic_DisplayCategoryName;
      errs() << "Diagnosis[" << i << "]="
             << getStrFromCXString(clang_formatDiagnostic(diag, formatOption))
             << '\n';
    }
  }
  clang_disposeCodeCompleteResults(compResults);
  clang_disposeTranslationUnit(tu);
  clang_disposeIndex(index);
  return 0;
}
