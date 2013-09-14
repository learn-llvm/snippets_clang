
#include <clang/CodeGen/CodeGenAction.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Frontend/CompilerInvocation.h>
#include <clang/Frontend/DiagnosticOptions.h>
#include <clang/Frontend/TextDiagnosticPrinter.h>
#include <llvm/ADT/IntrusiveRefCntPtr.h>
#include <llvm/ADT/OwningPtr.h>
#include <llvm/Module.h>

using namespace std;

int main(void) {
  // Path to the C file
  string inputPath = "getinmemory.c";

  // Arguments to pass to the clang frontend
  vector<const char *> args;
  args.push_back(inputPath.c_str());

  // The compiler invocation needs a DiagnosticsEngine so it can report problems
  clang::TextDiagnosticPrinter *DiagClient = new clang::TextDiagnosticPrinter(
      llvm::errs(), clang::DiagnosticOptions());
  llvm::IntrusiveRefCntPtr<clang::DiagnosticIDs> DiagID(
      new clang::DiagnosticIDs());
  clang::DiagnosticsEngine Diags(DiagID, DiagClient);

  // Create the compiler invocation
  llvm::OwningPtr<clang::CompilerInvocation> CI(new clang::CompilerInvocation);
  clang::CompilerInvocation::CreateFromArgs(*CI, &args[0],
                                            &args[0] + args.size(), Diags);

  // Print the argument list, which the compiler invocation has extended
  printf("clang ");
  vector<string> argsFromInvocation;
  CI->toArgs(argsFromInvocation);
  for (vector<string>::iterator i = argsFromInvocation.begin();
       i != argsFromInvocation.end(); ++i)
    printf("%s ", (*i).c_str());
  printf("\n");

  // Create the compiler instance
  clang::CompilerInstance Clang;
  Clang.setInvocation(CI.take());

  // Get ready to report problems
  Clang.createDiagnostics(args.size(), &args[0]);
  if (!Clang.hasDiagnostics()) return 1;

  // Create an action and make the compiler instance carry it out
  llvm::OwningPtr<clang::CodeGenAction> Act(new clang::EmitLLVMOnlyAction());
  if (!Clang.ExecuteAction(*Act)) return 1;

  // Grab the module built by the EmitLLVMOnlyAction
  llvm::Module *module = Act->takeModule();

  // Print all functions in the module
  for (llvm::Module::FunctionListType::iterator i =
           module->getFunctionList().begin();
       i != module->getFunctionList().end(); ++i)
    printf("%s\n", i->getName().str().c_str());

  return 0;
}
