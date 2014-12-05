#include "llvm/ADT/IntrusiveRefCntPtr.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Host.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Parse/Parser.h"
#include "clang/Parse/ParseAST.h"
#include <iostream>
using namespace llvm;
using namespace clang;
static cl::opt<std::string> FileName(cl::Positional, cl::desc("Input file"),
                                     cl::Required);
int main(int argc, char **argv) {
  cl::ParseCommandLineOptions(argc, argv, "My simple front end\n");
  CompilerInstance CI;
  DiagnosticOptions diagnosticOptions;
  CI.createDiagnostics();
  // IntrusiveRefCntPtr<TargetOptions> PTO(new TargetOptions());
  auto PTO = std::make_shared<TargetOptions>(TargetOptions());
  PTO->Triple = sys::getDefaultTargetTriple();
  TargetInfo *PTI = TargetInfo::CreateTargetInfo(CI.getDiagnostics(), PTO);
  CI.setTarget(PTI);

  CI.createFileManager();
  CI.createSourceManager(CI.getFileManager());
  CI.createPreprocessor(TranslationUnitKind::TU_Complete);
  CI.getPreprocessorOpts().UsePredefines = false;
  ASTConsumer *astConsumer = CreateASTPrinter(NULL, "");
  CI.setASTConsumer(astConsumer);
  CI.createASTContext();
  CI.createSema(TU_Complete, NULL);
  FileEntry const *pFile = CI.getFileManager().getFile(FileName);
  if (!pFile) {
    std::cerr << "File not found: " << FileName << std::endl;
    return 1;
  }
  SourceManager &sm = CI.getSourceManager();
  sm.createFileID(pFile, SourceLocation(), SrcMgr::C_User);
  CI.getDiagnosticClient().BeginSourceFile(CI.getLangOpts(), 0);
  ParseAST(CI.getSema());
  // Print AST statistics
  CI.getASTContext().PrintStats();
  CI.getASTContext().Idents.PrintStats();
  return 0;
}
