#include "Common.hpp"

#include <clang-c/Index.h>
#include <clang-c/CXCompilationDatabase.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/Format.h>

#define WITH_COLOR(color, x)         \
  {                                  \
    llvm::errs().changeColor(color); \
    x;                               \
    llvm::errs().resetColor();       \
  }

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
