#include <cstddef>
#include "clang/AST/ASTConsumer.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;

static llvm::cl::OptionCategory ToolingSampleCategory("Tooling Sample");

class MyASTConsumer : public ASTConsumer {
 public:
  MyASTConsumer(Rewriter &R) {}

  // This gets called *while the source is being parsed* - the full AST does not
  // exist yet.
  bool HandleTopLevelDecl(DeclGroupRef DR) override {
    for (auto &elem : DR) {
      elem->dump();
    }
    return true;
  }

  // This gets called only when the full TU is completely parsed.
  void HandleTranslationUnit(ASTContext &Context) override {
    llvm::errs() << "********* The whole TU *************\n";
    Context.getTranslationUnitDecl()->dump();

    llvm::errs() << "****** going over the decls stored in it:\n";
    for (auto *D : Context.getTranslationUnitDecl()->decls()) {
      llvm::errs() << "Decl in the TU:\n";
      D->dump();
      llvm::errs() << "Its start location is: '"
                   << D->getLocation().printToString(Context.getSourceManager())
                   << "'\n";
    }
  }
};

class MyFrontendAction : public ASTFrontendAction {
 public:
  MyFrontendAction() {}
  void EndSourceFileAction() override {
    SourceManager &SM = TheRewriter.getSourceMgr();
    llvm::errs() << "** EndSourceFileAction for: "
                 << SM.getFileEntryForID(SM.getMainFileID())->getName() << "\n";
  }

  std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                 StringRef file) override {
    SourceManager &SM = CI.getSourceManager();
    llvm::errs() << "** Creating AST consumer for: " << file << "\n";
    llvm::errs() << "  Main file ID: "
                 << SM.getFileEntryForID(SM.getMainFileID())->getName() << "\n";
    SM.PrintStats();
    TheRewriter.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return std::make_unique<MyASTConsumer>(TheRewriter);
  }

 private:
  Rewriter TheRewriter;
};

int main(int argc, const char **argv) {
  auto op =
      tooling::CommonOptionsParser::create(argc, argv, ToolingSampleCategory);
  if (auto E = op.takeError()) {
    llvm::errs() << E << "\n";
    return 1;
  }
  tooling::ClangTool Tool(op->getCompilations(), op->getSourcePathList());
  return Tool.run(tooling::newFrontendActionFactory<MyFrontendAction>().get());
}
