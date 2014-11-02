#include <cstdio>
#include <cstddef>
#include <memory>
#include <string>
#include <sstream>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/PrettyPrinter.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Parse/ParseAST.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Rewrite/Frontend/Rewriters.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace llvm;

// By implementing RecursiveASTVisitor, we can specify which AST nodes
// we're interested in by overriding relevant methods.
class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor> {
 public:
  std::string exprAsString(Expr *expr) {
    std::string s;
    raw_string_ostream ss(s);
    expr->printPretty(ss, nullptr, instance_.getASTContext().getLangOpts());
    return ss.str();
  }

  MyASTVisitor(Rewriter &R, CompilerInstance &instance)
      : rewriter_(R), instance_(instance) {}

  bool VisitIfStmt(IfStmt *ifStat) {
    Stmt *thenStat = ifStat->getThen();
    Expr *ifCond = ifStat->getCond();
    std::string ifComment = " /// if part: " + exprAsString(ifCond) + "\n";
    rewriter_.InsertText(thenStat->getLocStart(), ifComment, true, true);
    Stmt *elstStat = ifStat->getElse();
    if (elstStat) {
      std::string elseComment = " /// else part: !(" + elseComment + ")\n";
      rewriter_.InsertText(elstStat->getLocStart(), elseComment, true, true);
    }
    return true;
  }

  bool VisitWhileStmt(WhileStmt *s) {
    rewriter_.InsertText(s->getLocStart(), "/// start of while\n", true, true);
    rewriter_.InsertText(
        s->getLocEnd(),
        "/// end of while '(" + exprAsString(s->getCond()) + ")'\n", true,
        true);
    return true;
  }

  bool VisitSwitchStmt(SwitchStmt *s) { return true; }

  bool VisitCaseStmt(CaseStmt *s) { return true; }

  bool VisitDefaultStmt(DefaultStmt *s) { return true; }

  bool VisitStmt(Stmt *s) {
    auto stmtClass = s->getStmtClass();
    switch (stmtClass) {
      case Stmt::DeclStmtClass:
        break;
      case Stmt::NullStmtClass:
        break;
      case Stmt::CompoundStmtClass:
        break;
      case Stmt::CaseStmtClass:
        break;
      case Stmt::DefaultStmtClass:
        break;
      case Stmt::LabelStmtClass:
        break;
      case Stmt::AttributedStmtClass:
        break;
      case Stmt::DoStmtClass:
        break;
      case Stmt::GotoStmtClass:
        break;
      case Stmt::IndirectGotoStmtClass:
        break;
      case Stmt::ContinueStmtClass:
        break;
      case Stmt::BreakStmtClass:
        break;
      case Stmt::ReturnStmtClass:
        break;
      case Stmt::GCCAsmStmtClass:
        break;
      case Stmt::MSAsmStmtClass:
        break;
      case Stmt::SEHExceptStmtClass:
        break;
      case Stmt::SEHFinallyStmtClass:
        break;
      case Stmt::SEHTryStmtClass:
        break;
      case Stmt::SEHLeaveStmtClass:
        break;
      case Stmt::CapturedStmtClass:
        break;
      default:
        break;
    }
    return true;
  }

  bool VisitFunctionDecl(FunctionDecl *f) {
    if (!f->hasBody()) return true;
    Stmt *FuncBody = f->getBody();

    // Type name as string
    QualType QT = f->getReturnType();
    std::string typeStr = QT.getAsString();

    // Function name
    DeclarationName declName = f->getNameInfo().getName();
    std::string fnName = declName.getAsString();

    // Add comment before
    std::string s;
    raw_string_ostream ssBefore(s);
    ssBefore << "/// Begin function " << fnName << " returning " << typeStr
             << "\n";
    SourceLocation SL = f->getSourceRange().getBegin();
    rewriter_.InsertText(SL, ssBefore.str(), true, true);

    // And after
    std::stringstream ssAfter;
    ssAfter << "\n/// End function " << fnName;
    SL = FuncBody->getLocEnd().getLocWithOffset(1);
    rewriter_.InsertText(SL, ssAfter.str(), true, true);

    return true;
  }

 private:
  Rewriter &rewriter_;
  CompilerInstance &instance_;
};

// Implementation of the ASTConsumer interface for reading an AST produced
// by the Clang parser.
class MyASTConsumer : public ASTConsumer {
 public:
  MyASTConsumer(Rewriter &R, CompilerInstance &instance)
      : visitor_(R, instance) {}

  // Override the method that gets called for each parsed top-level
  // declaration.
  virtual bool HandleTopLevelDecl(DeclGroupRef DR) override {
    for (auto &elem : DR) visitor_.TraverseDecl(elem);
    return true;
  }

 private:
  MyASTVisitor visitor_;
};

int main(int argc, char *argv[]) {
  if (argc != 2) {
    errs() << "Usage:" << argv[0] << " <filename>\n";
    return 1;
  }

  // CompilerInstance will hold the instance of the Clang compiler for us,
  // managing the various objects needed to run the compiler.
  CompilerInstance instance;
  instance.createDiagnostics();

  LangOptions &LO = instance.getLangOpts();
  LO.CPlusPlus = 1;

  // Initialize target info with the default triple for our platform.
  auto TO = std::make_shared<TargetOptions>();
  TO->Triple = sys::getDefaultTargetTriple();
  TargetInfo *TI = TargetInfo::CreateTargetInfo(instance.getDiagnostics(), TO);
  instance.setTarget(TI);

  instance.createFileManager();
  FileManager &fileMgr = instance.getFileManager();
  instance.createSourceManager(fileMgr);
  SourceManager &srcMgr = instance.getSourceManager();
  instance.createPreprocessor(TU_Module);
  instance.createASTContext();

  // A Rewriter helps us manage the code rewriting task.
  Rewriter writer;
  writer.setSourceMgr(srcMgr, instance.getLangOpts());

  // Set the main file handled by the source manager to the input file.
  FileEntry const *fileEntry = fileMgr.getFile(argv[1]);
  srcMgr.setMainFileID(
      srcMgr.createFileID(fileEntry, SourceLocation(), SrcMgr::C_User));
  instance.getDiagnosticClient().BeginSourceFile(instance.getLangOpts(),
                                                 &instance.getPreprocessor());

  // Create an AST consumer instance which is going to get called by
  // ParseAST.
  MyASTConsumer consumer(writer, instance);

  // Parse the file to AST, registering our consumer as the AST consumer.
  ParseAST(instance.getPreprocessor(), &consumer, instance.getASTContext());

  // At this point the rewriter's buffer should be full with the rewritten
  // file contents.
  RewriteBuffer const *rewriteBuffer =
      writer.getRewriteBufferFor(srcMgr.getMainFileID());
  outs() << std::string(rewriteBuffer->begin(), rewriteBuffer->end());

  return 0;
}
