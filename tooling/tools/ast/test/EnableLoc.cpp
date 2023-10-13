// RUN: ast -l -f test "%s" 2>&1 | FileCheck %s

namespace test_namespace {

class TheClass {
 public:
  int theMethod(int x) { return x + x; }
};
}

// CHECK: NamespaceDecl <{{.*}}:3:1-12:1>
// CHECK-NEXT:   DeclarationName test_namespace <3:11>
// CHECK-NEXT:   CXXRecordDecl class <5:1-10:1>
// CHECK-NEXT:     DeclarationName TheClass <5:7>
// CHECK-NEXT:     AccessSpecDecl public <6:1-7>
// CHECK-NEXT:     CXXMethodDecl <7:3-9:3>
// CHECK-NEXT:       DeclarationName theMethod <7:7>
// CHECK-NEXT:       FunctionProtoType <7:3-22>
// CHECK-NEXT:         BuiltinType int <7:3>
// CHECK-NEXT:         ParmVarDecl <7:17-21>
// CHECK-NEXT:           DeclarationName x <7:21>
// CHECK-NEXT:           BuiltinType int <7:17>
// CHECK-NEXT:       CompoundStmt <7:24-9:3>
// CHECK-NEXT:         ReturnStmt <8:5-16>
// CHECK-NEXT:           BinaryOperator + <8:12-16>
// CHECK-NEXT:             ImplicitCastExpr LValueToRValue <8:12>
// CHECK-NEXT:               DeclRefExpr <8:12>
// CHECK-NEXT:                 DeclarationName x <8:12>
// CHECK-NEXT:             ImplicitCastExpr LValueToRValue <8:16>
// CHECK-NEXT:               DeclRefExpr <8:16>
// CHECK-NEXT:                 DeclarationName x <8:16>
