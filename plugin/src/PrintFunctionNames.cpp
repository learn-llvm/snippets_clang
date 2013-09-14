#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/AST.h"
#include "clang/Frontend/CompilerInstance.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
using namespace clang;

namespace {

class PrintFunctionsConsumer final : public ASTConsumer {
 public:
  virtual bool shouldSkipFunctionBody(Decl *D) { return false; }
  virtual bool HandleTopLevelDecl(DeclGroupRef DG) {
    for (DeclGroupRef::iterator i = DG.begin(), e = DG.end(); i != e; ++i) {
      const Decl *D = *i;
      if (const NamedDecl *ND = dyn_cast<NamedDecl>(D)) {
        errs() << "top-level-decl " << ND->getDeclName() << " ";
        ND->printQualifiedName(errs());
        errs() << "\n";
        if (isa<LabelDecl>(ND)) errs() << "label decl\n";
      }
    }

    return true;
  }
};

class PrintFunctionNamesAction : public PluginASTAction {
 protected:
  ASTConsumer *CreateASTConsumer(CompilerInstance &CI, StringRef) {
    return new PrintFunctionsConsumer();
  }

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) {
    for (unsigned i = 0, e = args.size(); i != e; ++i) {
      errs() << "PrintFunctionNames arg = " << args[i] << "\n";

      // Example error handling.
      if (args[i] == "-an-error") {
        DiagnosticsEngine &D = CI.getDiagnostics();
        unsigned DiagID = D.getCustomDiagID(
            DiagnosticsEngine::Error, "invalid argument '" + args[i] + "'");
        D.Report(DiagID);
        return false;
      }
    }
    if (args.size() && args[0] == "help") PrintHelp(errs());

    return true;
  }
  void PrintHelp(raw_ostream &ros) {
    ros << "Help for PrintFunctionNames plugin goes here\n";
  }
};
}

static FrontendPluginRegistry::Add<PrintFunctionNamesAction> X(
    "print-fns", "print function names");
