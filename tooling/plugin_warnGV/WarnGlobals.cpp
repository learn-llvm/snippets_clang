// -Wglobals Clang plugin

#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"

namespace {

class PrintGlobalsVisitor
    : public clang::RecursiveASTVisitor<PrintGlobalsVisitor> {
 public:
  explicit PrintGlobalsVisitor(clang::ASTContext *Context) : Context(Context) {}

  bool VisitVarDecl(clang::VarDecl *D) {
    const clang::SourceManager &SM = Context->getSourceManager();
    if (D->hasGlobalStorage() && !D->getType().isConstQualified()) {
      clang::FullSourceLoc loc = Context->getFullLoc(D->getLocStart());
      if (!SM.isInSystemHeader(loc)) {
        clang::DiagnosticsEngine &D = SM.getDiagnostics();
        unsigned int id = D.getCustomDiagID(clang::DiagnosticsEngine::Warning,
                                            "global variable");
        D.Report(loc, id);
      }
    }
    return true;
  }

 private:
  clang::ASTContext *Context;
};

class PrintGlobalsConsumer : public clang::ASTConsumer {
 public:
  explicit PrintGlobalsConsumer(clang::ASTContext *Context)
      : Visitor(Context) {}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }

 private:
  PrintGlobalsVisitor Visitor;
};

class PrintGlobalsAction : public clang::PluginASTAction {
 protected:
  clang::ASTConsumer *CreateASTConsumer(clang::CompilerInstance &CI,
                                        llvm::StringRef) {
    return new PrintGlobalsConsumer(&CI.getASTContext());
  }

  bool ParseArgs(const clang::CompilerInstance &CI,
                 const std::vector<std::string> &args) {
    // To be written...
    return true;
  }
};
}

static clang::FrontendPluginRegistry::Add<PrintGlobalsAction> X(
    "warn-globals", "generate warnings for non-const global variables");
