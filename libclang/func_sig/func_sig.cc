#include "clang-c/Index.h"
#include <cstdlib>
#include <string>
#include <set>
#include <iostream>

std::string GetClangString(CXString str) {
  const char* tmp = clang_getCString(str);
  if (tmp == NULL) {
    return "";
  } else {
    std::string translated = std::string(tmp);
    clang_disposeString(str);
    return translated;
  }
}

void GetMethodQualifiers(CXTranslationUnit translationUnit,
                         std::set<std::string>& qualifiers, CXCursor cursor) {
  qualifiers.clear();

  CXSourceRange range = clang_getCursorExtent(cursor);
  CXToken* tokens;
  unsigned int numTokens;
  clang_tokenize(translationUnit, range, &tokens, &numTokens);

  bool insideBrackets = false;
  for (unsigned int i = 0; i < numTokens; i++) {
    std::string token =
        GetClangString(clang_getTokenSpelling(translationUnit, tokens[i]));
    if (token == "(") {
      insideBrackets = true;
    } else if (token == "{" || token == ";") {
      break;
    } else if (token == ")") {
      insideBrackets = false;
    } else if (clang_getTokenKind(tokens[i]) == CXToken_Keyword &&
               !insideBrackets) {
      qualifiers.insert(token);
    }
  }

  clang_disposeTokens(translationUnit, tokens, numTokens);
}

int main(int argc, char* argv[]) {
  CXIndex Index = clang_createIndex(0, 0);
  unsigned line = atoi(argv[2]);
  unsigned column = atoi(argv[3]);
  CXTranslationUnit TU = clang_parseTranslationUnit(
      Index, 0, argv + 3, argc - 3, 0, 0, CXTranslationUnit_None);
  CXFile file = clang_getFile(TU, argv[1]);
  printf("%s:%u:%u\n", argv[1], line, column);

  CXSourceLocation location = clang_getLocation(TU, file, line, column);
  CXCursor cursor = clang_getCursor(TU, location);
  enum CXCursorKind kind = clang_getCursorKind(cursor);
  CXString cx_str_kind = clang_getCursorKindSpelling(kind);
  printf("%s", clang_getCString(cx_str_kind));
  clang_disposeString(cx_str_kind);

  std::set<std::string> qualifiers;
  GetMethodQualifiers(TU, qualifiers, cursor);

  for (std::set<std::string>::const_iterator i = qualifiers.begin();
       i != qualifiers.end(); ++i) {
    std::cout << *i << std::endl;
  }

  clang_disposeTranslationUnit(TU);
  clang_disposeIndex(Index);
  return 0;
}
