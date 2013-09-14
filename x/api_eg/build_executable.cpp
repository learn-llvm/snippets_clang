#include <clang/Driver/Compilation.h>
#include <clang/Driver/Driver.h>
#include <clang/Frontend/DiagnosticOptions.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/Program.h>
#include <llvm/Support/raw_ostream.h>

using namespace std;

int main(void) {
  // Path to the C file
  string inputPath = "getinmemory.c";

  // Path to the executable
  string outputPath = "getinmemory";

  // Path to clang (e.g. /usr/local/bin/clang)
  llvm::sys::Path clangPath = llvm::sys::Program::FindProgramByName("clang");

  // Arguments to pass to the clang driver:
  //	clang getinmemory.c -lcurl -v
  vector<const char *> args;
  args.push_back(clangPath.c_str());
  args.push_back(inputPath.c_str());
  args.push_back("-l");
  args.push_back("curl");
  args.push_back("-v");  // verbose

  // The clang driver needs a DiagnosticsEngine so it can report problems
  clang::TextDiagnosticPrinter *DiagClient = new clang::TextDiagnosticPrinter(
      llvm::errs(), clang::DiagnosticOptions());
  clang::IntrusiveRefCntPtr<clang::DiagnosticIDs> DiagID(
      new clang::DiagnosticIDs());
  clang::DiagnosticsEngine Diags(DiagID, DiagClient);

  // Create the clang driver
  clang::driver::Driver TheDriver(args[0], llvm::sys::getDefaultTargetTriple(),
                                  outputPath, true, Diags);

  // If you want to build C++ instead of C
  //	TheDriver.CCCIsCXX = true;

  // Create the set of actions to perform
  clang::OwningPtr<clang::driver::Compilation> C(
      TheDriver.BuildCompilation(args));

  // Print the set of actions
  TheDriver.PrintActions(*C);

  // Carry out the actions
  int Res = 0;
  const clang::driver::Command *FailingCommand = 0;
  if (C) Res = TheDriver.ExecuteCompilation(*C, FailingCommand);

  // Report problems
  if (Res < 0) TheDriver.generateCompilationDiagnostics(*C, FailingCommand);
}
