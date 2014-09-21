#include <string>

#include "clang/AST/AST.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;

static llvm::cl::OptionCategory ToolingSampleCategory("Tooling Sample");

class IfStmtHandler : public ast_matchers::MatchFinder::MatchCallback {
 public:
  IfStmtHandler(tooling::Replacements *Replace) : Replace(Replace) {}

  virtual void run(const ast_matchers::MatchFinder::MatchResult &Result)
      override {
    // The matched 'if' statement was bound to 'ifStmt'.
    if (auto *IfS = Result.Nodes.getNodeAs<IfStmt>("ifStmt")) {
      const Stmt *Then = IfS->getThen();
      tooling::Replacement Rep(*(Result.SourceManager), Then->getLocStart(), 0,
                               "// the 'if' part\n");
      Replace->insert(Rep);

      if (auto *Else = IfS->getElse()) {
        tooling::Replacement Rep(*(Result.SourceManager), Else->getLocStart(),
                                 0, "// the 'else' part\n");
        Replace->insert(Rep);
      }
    }
  }

 private:
  tooling::Replacements *Replace;
};

int main(int argc, const char **argv) {
  tooling::CommonOptionsParser op(argc, argv, ToolingSampleCategory);
  tooling::RefactoringTool Tool(op.getCompilations(), op.getSourcePathList());

  // Set up AST matcher callbacks.
  IfStmtHandler HandlerForIf(&Tool.getReplacements());

  ast_matchers::MatchFinder Finder;
  Finder.addMatcher(ast_matchers::ifStmt().bind("ifStmt"), &HandlerForIf);

  // Run the tool and collect a list of replacements. We could call runAndSave,
  // which would destructively overwrite the files with their new contents.
  // However, for demonstration purposes it's interesting to print out the
  // would-be contents of the rewritten files instead of actually rewriting
  // them.
  if (int Result = Tool.run(tooling::newFrontendActionFactory(&Finder).get())) {
    return Result;
  }

  // We need a SourceManager to set up the Rewriter.
  IntrusiveRefCntPtr<DiagnosticOptions> DiagOpts = new DiagnosticOptions();
  DiagnosticsEngine Diagnostics(
      IntrusiveRefCntPtr<DiagnosticIDs>(new DiagnosticIDs()), &*DiagOpts,
      new TextDiagnosticPrinter(llvm::errs(), &*DiagOpts), true);
  SourceManager Sources(Diagnostics, Tool.getFiles());

  // Apply all replacements to a rewriter.
  Rewriter Rewrite(Sources, LangOptions());
  Tool.applyAllReplacements(Rewrite);

  // Query the rewriter for all the files it has rewritten, dumping their new
  // contents to stdout.
  for (auto I = Rewrite.buffer_begin(), E = Rewrite.buffer_end(); I != E; ++I) {
    const FileEntry *Entry = Sources.getFileEntryForID(I->first);
    llvm::outs() << "Rewrite buffer for file: " << Entry->getName() << "\n";
    I->second.write(llvm::outs());
  }

  return 0;
}
