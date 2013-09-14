#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/AST.h"
#include "clang/Frontend/CompilerInstance.h"
#include "llvm/Support/raw_ostream.h"
using namespace clang;
using namespace llvm;

namespace {

class PrintFunctionsConsumer : public ASTConsumer {
 public:
  virtual bool HandleTopLevelDecl(DeclGroupRef DG) {
    for (DeclGroupRef::iterator i = DG.begin(), e = DG.end(); i != e; ++i) {
      const Decl *D = *i;
      if (const NamedDecl *ND = dyn_cast<NamedDecl>(D)) {
        errs() << "top-level declaration:\n" << ND->getNameAsString() << "\"\n";
        IdentifierInfo *IdInfo = ND->getIdentifier();
        tok::TokenKind tk = IdInfo->getTokenID();
        const char *tname = tok::getTokenName(tk);
        errs() << "token name is:";
        errs().write_escaped(tname);
        errs() << "\n";
        if (const FunctionDecl *FD = dyn_cast<FunctionDecl>(D)) {
          errs() << "function declaration:\n";
          QualType qt = FD->getResultType();
          SplitQualType sqt = qt.split();
          errs().write_escaped(QualType::getAsString(sqt));
          errs() << "\n";
        }

        bool isadd = IdInfo->isStr("add");
        if (isadd) {
          errs() << "This is add declaration\n";
        }
        if (const VarDecl *VD = dyn_cast<VarDecl>(D)) {
          errs() << "Variable-declaration:\n";
          QualType qt = VD->getType();
          SplitQualType sqt = qt.split();
          errs().write_escaped(QualType::getAsString(sqt));
          errs() << "\n";
        }
        errs() << "\n";
      }  //if
    }    //for

    return true;
  }  //HandleTopLevelDecl
};   //namespace

class SimpleFuncPluginAction : public PluginASTAction {
 protected:
  ASTConsumer *CreateASTConsumer(CompilerInstance &CI, StringRef) {
    return new PrintFunctionsConsumer();
  }

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string> &args) {
    for (unsigned i = 0, e = args.size(); i != e; ++i) {
      errs() << "SimpleFuncPlugin arg = " << args[i] << "\n";

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
    ros << "Help for SimpleFuncPlugin plugin goes here\n";
  }
};
}

static FrontendPluginRegistry::Add<SimpleFuncPluginAction> X(
    "SimpleFuncPlugin", "SimpleFuncPlugin");
