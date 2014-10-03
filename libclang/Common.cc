#include "Common.hpp"

#include "clang-c/CXCompilationDatabase.h"

#include <set>
#include <map>
#include <string>

using namespace llvm;

std::string getStrFromCXString(CXString const &cxstring) {
  std::string str;
  char const *cstr = clang_getCString(cxstring);
  if (cstr != nullptr) str = cstr;
  clang_disposeString(cxstring);
  return str;
}

int DBParser(int argc, char **argv) {
  if (argc != 2) {
    errs() << format("%s database_dir\n", argv[0]);
    exit(1);
  }

  CXCompilationDatabase_Error error;
  CXCompilationDatabase db =
      clang_CompilationDatabase_fromDirectory(argv[1], &error);
  switch (error) {
    case CXCompilationDatabase_NoError:
      break;
    case CXCompilationDatabase_CanNotLoadDatabase:
      errs() << "Cannot load database\n";
      exit(1);
    default:
      errs() << "unknown return\n";
      exit(1);
  }

  auto cmds = clang_CompilationDatabase_getAllCompileCommands(db);
  auto numCmds = clang_CompileCommands_getSize(cmds);
  WITH_COLOR(raw_ostream::BLUE, errs() << numCmds << '\n';);
  for (auto i = 0U; i < numCmds; i++) {
    CXCompileCommands cmd = clang_CompileCommands_getCommand(cmds, i);
    auto numArgs = clang_CompileCommand_getNumArgs(cmd);
    WITH_COLOR(raw_ostream::YELLOW, errs() << numArgs << '\n');
    for (auto j = 0U; j < numArgs; j++) {
      CXString arg = clang_CompileCommand_getArg(cmd, j);
      errs() << clang_getCString(arg) << '\n';
      clang_disposeString(arg);
    }
  }

  clang_CompileCommands_dispose(cmds);
  clang_CompilationDatabase_dispose(db);
  return 0;
}

auto getFnQualifiers(CXTranslationUnit translationUnit, CXCursor cursor) {
  assert(!clang_isInvalid(clang_getCursorKind(cursor)));

  std::set<std::string> qualifiers;
  auto range = clang_getCursorExtent(cursor);
  CXToken *tokens;
  unsigned numTokens;
  clang_tokenize(translationUnit, range, &tokens, &numTokens);

  bool insideBrackets = false;
  for (unsigned i = 0u; i < numTokens; i++) {
    auto tokenKind = clang_getTokenKind(tokens[i]);
    auto token =
        getStrFromCXString(clang_getTokenSpelling(translationUnit, tokens[i]));
    if (token == "(") {
      insideBrackets = true;
    } else if (token == "{" || token == ";") {
      break;
    } else if (token == ")") {
      insideBrackets = false;
    } else if (tokenKind == CXToken_Keyword && !insideBrackets) {
      qualifiers.insert(token);
    }
  }
  clang_disposeTokens(translationUnit, tokens, numTokens);
  return qualifiers;
}

void dumpTokenInRange(const CXTranslationUnit &tu, CXSourceRange range) {

  typedef std::map<CXTokenKind, std::string> TokKind2StrMap;
#define TokPair(name) \
  { CXToken_##name, #name }

  static TokKind2StrMap tokenMap{TokPair(Punctuation), TokPair(Keyword),
                                 TokPair(Identifier),  TokPair(Literal),
                                 TokPair(Comment)};
#undef TokPair

  CXToken *tokens;
  unsigned numTokens;
  clang_tokenize(tu, range, &tokens, &numTokens);
  for (auto i = 0U; i < numTokens; i++) {
    auto const &token = tokens[i];
    auto kind = clang_getTokenKind(token);
    auto spell = clang_getTokenSpelling(tu, token);
    CXSourceLocation loc = clang_getTokenLocation(tu, token);

    CXFile file;
    unsigned line, column, offset;
    clang_getFileLocation(loc, &file, &line, &column, &offset);

    errs() << "Token: " << i << '\n';
    errs() << "Text: " << getStrFromCXString(spell) << '\n';
    errs() << "Kind: " << tokenMap[kind] << '\n';
    errs() << format("Location: %s:%d:%d:%d\n",
                     getStrFromCXString(clang_getFileName(file)).data(), line,
                     column, offset);
    errs() << '\n';
  }
  clang_disposeTokens(tu, tokens, numTokens);
}

void dumpTokenInTU(CXTranslationUnit tu) {
  auto range = clang_getCursorExtent(clang_getTranslationUnitCursor(tu));
  dumpTokenInRange(tu, range);
}
