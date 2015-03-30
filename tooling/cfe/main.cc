#include "llvm/ADT/StringMap.h"
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

using namespace llvm;
using namespace clang;
static cl::opt<std::string> FileName(cl::Positional, cl::desc("Input file"),
                                     cl::Required);

int main(int argc, char **argv) {
  cl::ParseCommandLineOptions(argc, argv, "My simple front end\n");
  CompilerInstance CI;
  DiagnosticOptions diagnosticOptions;
  CI.createDiagnostics(nullptr, true);
  auto PTO = std::make_shared<TargetOptions>(TargetOptions());
  PTO->Triple = sys::getDefaultTargetTriple();
  {
    llvm::errs() << "targetTriple:" << sys::getDefaultTargetTriple() << '\n'
                 << "cpu name: " << sys::getHostCPUName() << '\n'
                 << "process triple: " << sys::getProcessTriple() << '\n'
                 << "Big endian: " << sys::IsBigEndianHost << '\n';
    StringMap<bool> strMap;
    bool succeed = sys::getHostCPUFeatures(strMap);
    if (succeed) {
      errs() << "features: ";
      for (auto &feature : strMap) {
        errs() << feature.getKey() << ": " << feature.getValue();
      }
    }
  }
  TargetInfo *PTI = TargetInfo::CreateTargetInfo(CI.getDiagnostics(), PTO);
  CI.setTarget(PTI);

  CI.createFileManager();
  FileManager &filemgr = CI.getFileManager();
  CI.createSourceManager(filemgr);
  CI.createPreprocessor(TranslationUnitKind::TU_Complete);
  CI.getPreprocessorOpts().UsePredefines = false;
  std::unique_ptr<ASTConsumer> astConsumer = CreateASTPrinter(NULL, "");
  CI.setASTConsumer(std::move(astConsumer));
  CI.createASTContext();
  CI.createSema(TU_Complete, NULL);
  {
    errs() << "hasdiagnostics: " << CI.hasDiagnostics() << '\n';
    errs() << "hasinvocation: " << CI.hasInvocation() << '\n';
    {
      auto &headerSearchOpts = CI.getHeaderSearchOpts();
      errs() << "sysroot: " << headerSearchOpts.Sysroot << '\n';
      errs() << "resourceDir: " << headerSearchOpts.ResourceDir << '\n';
      errs() << "ModuleUserBuildPath: " << headerSearchOpts.ModuleUserBuildPath
             << '\n';
      errs() << "header user entries:\t";
      for (auto &entry : headerSearchOpts.UserEntries) {
        errs() << entry.Path << "  ";
      }
      errs() << '\n';
      errs() << "system header entries:\t";
      for (auto &entry : headerSearchOpts.SystemHeaderPrefixes) {
        errs() << entry.Prefix << ": " << entry.IsSystemHeader << '\t';
      }
      errs() << '\n';
      for (auto &ignoredMacro : headerSearchOpts.ModulesIgnoreMacros) {
        errs() << ignoredMacro << '\n';
      }
    }
  }
  FileEntry const *pFile = CI.getFileManager().getFile(FileName);
  if (!pFile) {
    errs() << "File not found: " << FileName << '\n';
    return 1;
  }
  {
    errs() << "name: " << pFile->getName() << '\n';
    errs() << "dir: " << pFile->getDir()->getName() << '\n';
    errs() << "size: " << pFile->getSize() << '\n';
    errs() << "uid:" << pFile->getUID() << " uniqueID: " << pFile->getUID()
           << '\n';
  }
  SourceManager &sm = CI.getSourceManager();
  sm.setMainFileID(sm.createFileID(pFile, SourceLocation(), SrcMgr::C_User));
  CI.getDiagnosticClient().BeginSourceFile(CI.getLangOpts(), 0);
  /// ParseAST(CI.getSema());
  // Print AST statistics
  /// CI.getASTContext().PrintStats();
  CI.getASTContext().Idents.PrintStats();
  return 0;
}
