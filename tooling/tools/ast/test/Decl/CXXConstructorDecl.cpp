// RUN: ast -f test "%s" 2>&1 | FileCheck %s

class test1 {
  // CHECK:   CXXConstructorDecl
  // CHECK-NEXT:     DeclarationName test1
  // CHECK-NEXT:     FunctionProtoType
  // CHECK-NEXT:       BuiltinType void
  test1();
};

// CHECK: CXXConstructorDecl
// CHECK-NEXT:   NestedNameSpecifier test1::
// CHECK-NEXT:     RecordType test1
// CHECK-NEXT:   DeclarationName test1
// CHECK-NEXT:   FunctionProtoType
// CHECK-NEXT:     BuiltinType void
// CHECK-NEXT:   CompoundStmt
test1::test1() {}

class test2 {
  // CHECK:   CXXConstructorDecl
  // CHECK-NEXT:     DeclarationName test2
  // CHECK-NEXT:     FunctionProtoType
  // CHECK-NEXT:       BuiltinType void
  // CHECK-NEXT:       ParmVarDecl
  // CHECK-NEXT:         DeclarationName a
  // CHECK-NEXT:         BuiltinType int
  // CHECK-NEXT:     CompoundStmt
  test2(int a) {}
};

class A {};

class test3 : public A {
  int I;
  // CHECK:   CXXConstructorDecl
  // CHECK-NEXT:     DeclarationName test3
  // CHECK-NEXT:     FunctionProtoType
  // CHECK-NEXT:       BuiltinType void
  // CHECK-NEXT:       ParmVarDecl
  // CHECK-NEXT:         DeclarationName a
  // CHECK-NEXT:         LValueReferenceType
  // CHECK-NEXT:           RecordType A
  // CHECK-NEXT:       ParmVarDecl
  // CHECK-NEXT:         DeclarationName i
  // CHECK-NEXT:         BuiltinType int
  // CHECK-NEXT:     CXXCtorInitializer
  // CHECK-NEXT:       RecordType A
  // CHECK-NEXT:       CXXConstructExpr
  // CHECK-NEXT:         ImplicitCastExpr NoOp
  // CHECK-NEXT:           DeclRefExpr
  // CHECK-NEXT:             DeclarationName a
  // CHECK-NEXT:     CXXCtorInitializer
  // CHECK-NEXT:       FieldDeclRef I
  // CHECK-NEXT:       ImplicitCastExpr LValueToRValue
  // CHECK-NEXT:         DeclRefExpr
  // CHECK-NEXT:           DeclarationName i
  // CHECK-NEXT:     CompoundStmt
  test3(A& a, int i) : A(a), I(i) {}
};

class test4 : public A {
  // CHECK:   CXXConstructorDecl
  // CHECK-NEXT:     DeclarationName test4
  // CHECK-NEXT:     FunctionProtoType
  // CHECK-NEXT:       BuiltinType void
  // CHECK-NEXT:     CompoundStmt
  test4() {}
};
