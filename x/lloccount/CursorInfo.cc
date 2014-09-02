#include <clang-c/Index.h>
#include <cstdio>
#include <map>

#define MAKE_COUNTER_PAIR(key) \
  { key, 0u }

typedef std::map<CXCursorKind, unsigned> CursorInfo;
const char *filename;

CursorInfo counter{
    MAKE_COUNTER_PAIR(CXCursor_UnexposedDecl),
    MAKE_COUNTER_PAIR(CXCursor_StructDecl),
    MAKE_COUNTER_PAIR(CXCursor_UnionDecl),
    MAKE_COUNTER_PAIR(CXCursor_ClassDecl),
    MAKE_COUNTER_PAIR(CXCursor_EnumDecl),
    MAKE_COUNTER_PAIR(CXCursor_IfStmt),
    MAKE_COUNTER_PAIR(CXCursor_SwitchStmt),
    MAKE_COUNTER_PAIR(CXCursor_WhileStmt),
    MAKE_COUNTER_PAIR(CXCursor_DoStmt),
    MAKE_COUNTER_PAIR(CXCursor_ForStmt),
    MAKE_COUNTER_PAIR(CXCursor_GotoStmt),
    MAKE_COUNTER_PAIR(CXCursor_BreakStmt),
    MAKE_COUNTER_PAIR(CXCursor_ReturnStmt),
    MAKE_COUNTER_PAIR(CXCursor_InclusionDirective),
    MAKE_COUNTER_PAIR(CXCursor_CallExpr),
    MAKE_COUNTER_PAIR(CXCursor_FunctionDecl),
    MAKE_COUNTER_PAIR(CXCursor_VarDecl),
    MAKE_COUNTER_PAIR(CXCursor_ParmDecl),
    MAKE_COUNTER_PAIR(CXCursor_FieldDecl),
    MAKE_COUNTER_PAIR(CXCursor_EnumConstantDecl),
    MAKE_COUNTER_PAIR(CXCursor_ObjCInterfaceDecl),
    MAKE_COUNTER_PAIR(CXCursor_ObjCCategoryDecl),
    MAKE_COUNTER_PAIR(CXCursor_ObjCProtocolDecl),
    MAKE_COUNTER_PAIR(CXCursor_ObjCPropertyDecl),
    MAKE_COUNTER_PAIR(CXCursor_ObjCIvarDecl),
    MAKE_COUNTER_PAIR(CXCursor_ObjCInstanceMethodDecl),
    MAKE_COUNTER_PAIR(CXCursor_ObjCClassMethodDecl),
    MAKE_COUNTER_PAIR(CXCursor_ObjCImplementationDecl),
    MAKE_COUNTER_PAIR(CXCursor_ObjCCategoryImplDecl),
    MAKE_COUNTER_PAIR(CXCursor_TypedefDecl),
    MAKE_COUNTER_PAIR(CXCursor_CXXMethod),
    MAKE_COUNTER_PAIR(CXCursor_Namespace),
    MAKE_COUNTER_PAIR(CXCursor_LinkageSpec),
    MAKE_COUNTER_PAIR(CXCursor_Constructor),
    MAKE_COUNTER_PAIR(CXCursor_Destructor),
    MAKE_COUNTER_PAIR(CXCursor_ConversionFunction),
    MAKE_COUNTER_PAIR(CXCursor_TemplateTypeParameter),
    MAKE_COUNTER_PAIR(CXCursor_NonTypeTemplateParameter),
    MAKE_COUNTER_PAIR(CXCursor_TemplateTemplateParameter),
    MAKE_COUNTER_PAIR(CXCursor_FunctionTemplate),
    MAKE_COUNTER_PAIR(CXCursor_ClassTemplate),
    MAKE_COUNTER_PAIR(CXCursor_ClassTemplatePartialSpecialization),
    MAKE_COUNTER_PAIR(CXCursor_NamespaceAlias),
    MAKE_COUNTER_PAIR(CXCursor_UsingDirective),
    MAKE_COUNTER_PAIR(CXCursor_UsingDeclaration),
    MAKE_COUNTER_PAIR(CXCursor_TypeAliasDecl),
    MAKE_COUNTER_PAIR(CXCursor_ObjCSynthesizeDecl),
    MAKE_COUNTER_PAIR(CXCursor_ObjCDynamicDecl),
    MAKE_COUNTER_PAIR(CXCursor_CXXAccessSpecifier),
    MAKE_COUNTER_PAIR(CXCursor_FirstRef),
    MAKE_COUNTER_PAIR(CXCursor_ObjCSuperClassRef),
    MAKE_COUNTER_PAIR(CXCursor_ObjCProtocolRef),
    MAKE_COUNTER_PAIR(CXCursor_ObjCClassRef),
    MAKE_COUNTER_PAIR(CXCursor_TypeRef),
    MAKE_COUNTER_PAIR(CXCursor_CXXBaseSpecifier),
    MAKE_COUNTER_PAIR(CXCursor_TemplateRef),
    MAKE_COUNTER_PAIR(CXCursor_NamespaceRef),
    MAKE_COUNTER_PAIR(CXCursor_MemberRef),
    MAKE_COUNTER_PAIR(CXCursor_LabelRef),
    MAKE_COUNTER_PAIR(CXCursor_OverloadedDeclRef),
    MAKE_COUNTER_PAIR(CXCursor_FirstInvalid),
    MAKE_COUNTER_PAIR(CXCursor_InvalidFile),
    MAKE_COUNTER_PAIR(CXCursor_NoDeclFound),
    MAKE_COUNTER_PAIR(CXCursor_NotImplemented),
    MAKE_COUNTER_PAIR(CXCursor_InvalidCode),
    MAKE_COUNTER_PAIR(CXCursor_FirstExpr),
    MAKE_COUNTER_PAIR(CXCursor_UnexposedExpr),
    MAKE_COUNTER_PAIR(CXCursor_DeclRefExpr),
    MAKE_COUNTER_PAIR(CXCursor_MemberRefExpr),
    MAKE_COUNTER_PAIR(CXCursor_ObjCMessageExpr),
    MAKE_COUNTER_PAIR(CXCursor_BlockExpr),
    MAKE_COUNTER_PAIR(CXCursor_IntegerLiteral),
    MAKE_COUNTER_PAIR(CXCursor_FloatingLiteral),
    MAKE_COUNTER_PAIR(CXCursor_ImaginaryLiteral),
    MAKE_COUNTER_PAIR(CXCursor_StringLiteral),
    MAKE_COUNTER_PAIR(CXCursor_CharacterLiteral),
    MAKE_COUNTER_PAIR(CXCursor_ParenExpr),
    MAKE_COUNTER_PAIR(CXCursor_UnaryOperator),
    MAKE_COUNTER_PAIR(CXCursor_ArraySubscriptExpr),
    MAKE_COUNTER_PAIR(CXCursor_BinaryOperator),
    MAKE_COUNTER_PAIR(CXCursor_CompoundAssignOperator),
    MAKE_COUNTER_PAIR(CXCursor_ConditionalOperator),
    MAKE_COUNTER_PAIR(CXCursor_CStyleCastExpr),
    MAKE_COUNTER_PAIR(CXCursor_CompoundLiteralExpr),
    MAKE_COUNTER_PAIR(CXCursor_InitListExpr),
    MAKE_COUNTER_PAIR(CXCursor_AddrLabelExpr),
    MAKE_COUNTER_PAIR(CXCursor_StmtExpr),
    MAKE_COUNTER_PAIR(CXCursor_GenericSelectionExpr),
    MAKE_COUNTER_PAIR(CXCursor_GNUNullExpr),
    MAKE_COUNTER_PAIR(CXCursor_CXXStaticCastExpr),
    MAKE_COUNTER_PAIR(CXCursor_CXXDynamicCastExpr),
    MAKE_COUNTER_PAIR(CXCursor_CXXReinterpretCastExpr),
    MAKE_COUNTER_PAIR(CXCursor_CXXConstCastExpr),
    MAKE_COUNTER_PAIR(CXCursor_CXXFunctionalCastExpr),
    MAKE_COUNTER_PAIR(CXCursor_CXXTypeidExpr),
    MAKE_COUNTER_PAIR(CXCursor_CXXBoolLiteralExpr),
    MAKE_COUNTER_PAIR(CXCursor_CXXNullPtrLiteralExpr),
    MAKE_COUNTER_PAIR(CXCursor_CXXThisExpr),
    MAKE_COUNTER_PAIR(CXCursor_CXXThrowExpr),
    MAKE_COUNTER_PAIR(CXCursor_CXXNewExpr),
    MAKE_COUNTER_PAIR(CXCursor_CXXDeleteExpr),
    MAKE_COUNTER_PAIR(CXCursor_UnaryExpr),
    MAKE_COUNTER_PAIR(CXCursor_ObjCStringLiteral),
    MAKE_COUNTER_PAIR(CXCursor_ObjCEncodeExpr),
    MAKE_COUNTER_PAIR(CXCursor_ObjCSelectorExpr),
    MAKE_COUNTER_PAIR(CXCursor_ObjCProtocolExpr),
    MAKE_COUNTER_PAIR(CXCursor_ObjCBridgedCastExpr),
    MAKE_COUNTER_PAIR(CXCursor_PackExpansionExpr),
    MAKE_COUNTER_PAIR(CXCursor_SizeOfPackExpr),
    MAKE_COUNTER_PAIR(CXCursor_FirstStmt),
    MAKE_COUNTER_PAIR(CXCursor_UnexposedStmt),
    MAKE_COUNTER_PAIR(CXCursor_LabelStmt),
    MAKE_COUNTER_PAIR(CXCursor_CompoundStmt),
    MAKE_COUNTER_PAIR(CXCursor_CaseStmt),
    MAKE_COUNTER_PAIR(CXCursor_DefaultStmt),
    MAKE_COUNTER_PAIR(CXCursor_IndirectGotoStmt),
    MAKE_COUNTER_PAIR(CXCursor_ContinueStmt),
    MAKE_COUNTER_PAIR(CXCursor_AsmStmt),
    MAKE_COUNTER_PAIR(CXCursor_ObjCAtTryStmt),
    MAKE_COUNTER_PAIR(CXCursor_ObjCAtCatchStmt),
    MAKE_COUNTER_PAIR(CXCursor_ObjCAtFinallyStmt),
    MAKE_COUNTER_PAIR(CXCursor_ObjCAtThrowStmt),
    MAKE_COUNTER_PAIR(CXCursor_ObjCAtSynchronizedStmt),
    MAKE_COUNTER_PAIR(CXCursor_ObjCAutoreleasePoolStmt),
    MAKE_COUNTER_PAIR(CXCursor_ObjCForCollectionStmt),
    MAKE_COUNTER_PAIR(CXCursor_CXXCatchStmt),
    MAKE_COUNTER_PAIR(CXCursor_CXXTryStmt),
    MAKE_COUNTER_PAIR(CXCursor_CXXForRangeStmt),
    MAKE_COUNTER_PAIR(CXCursor_SEHTryStmt),
    MAKE_COUNTER_PAIR(CXCursor_SEHExceptStmt),
    MAKE_COUNTER_PAIR(CXCursor_SEHFinallyStmt),
    MAKE_COUNTER_PAIR(CXCursor_NullStmt),
    MAKE_COUNTER_PAIR(CXCursor_DeclStmt),
    MAKE_COUNTER_PAIR(CXCursor_TranslationUnit),
    MAKE_COUNTER_PAIR(CXCursor_FirstAttr),
    MAKE_COUNTER_PAIR(CXCursor_UnexposedAttr),
    MAKE_COUNTER_PAIR(CXCursor_IBActionAttr),
    MAKE_COUNTER_PAIR(CXCursor_IBOutletAttr),
    MAKE_COUNTER_PAIR(CXCursor_IBOutletCollectionAttr),
    MAKE_COUNTER_PAIR(CXCursor_CXXFinalAttr),
    MAKE_COUNTER_PAIR(CXCursor_CXXOverrideAttr),
    MAKE_COUNTER_PAIR(CXCursor_AnnotateAttr),
    MAKE_COUNTER_PAIR(CXCursor_PreprocessingDirective),
    MAKE_COUNTER_PAIR(CXCursor_MacroDefinition),
    MAKE_COUNTER_PAIR(CXCursor_MacroExpansion), };

void printDecl(CXCursor cursor) {
  CXString cursorCXStr = clang_getCursorSpelling(cursor);
  const char *cursorStr = clang_getCString(cursorCXStr);
  CXString objcTypeCXStr = clang_getDeclObjCTypeEncoding(cursor);
  const char *objcTypeStr = clang_getCString(objcTypeCXStr);
  if (!cursorStr) cursorStr = "Unknown";
  if (!objcTypeStr) objcTypeStr = "Unknown";
  clang_disposeString(cursorCXStr);
  clang_disposeString(objcTypeCXStr);
}

std::string getFileName(CXCursor cursor) {
  CXFile file;
  clang_getSpellingLocation(clang_getCursorLocation(cursor), &file, 0, 0, 0);
  CXString fileCXStr = clang_getFileName(file);
  const char *fileStr = clang_getCString(fileCXStr);
  if (fileStr == nullptr) return "";
  std::string fileName{fileStr};
  clang_disposeString(fileCXStr);
  return fileName;
}

CXChildVisitResult statsVisitor(CXCursor cursor, CXCursor parent,
                                CXClientData client_data) {
  std::string &&file = getFileName(cursor);
  if (file == filename) {
    ++counter[cursor.kind];
    printDecl(cursor);
  }
  return CXChildVisit_Recurse;
}

int main(int argc, char **argv) {
  std::printf("%s\n", clang_getCString(
                          clang_getCursorKindSpelling(CXCursor_UnexposedDecl)));
  if (argc < 2) {
    std::fprintf(stderr, "usage: %s ${c_cxx_src_file}", argv[0]);
    std::exit(1);
  }
  filename = argv[1];
  CXIndex idx = clang_createIndex(1, 0);
  CXTranslationUnit TU = clang_parseTranslationUnit(
      idx, filename, argv + 2, argc - 2, 0, 0,
      CXTranslationUnit_PrecompiledPreamble |
          CXTranslationUnit_DetailedPreprocessingRecord);
  clang_reparseTranslationUnit(TU, 0, 0, 0);
  clang_visitChildren(clang_getTranslationUnitCursor(TU), statsVisitor,
                      nullptr);
  for (auto const &ele : counter) {
    CXString cursorCXStr = clang_getCursorKindSpelling(ele.first);
    std::printf("%s: %d\n", clang_getCString(cursorCXStr), ele.second);
    clang_disposeString(cursorCXStr);
  }
  return 0;
}
