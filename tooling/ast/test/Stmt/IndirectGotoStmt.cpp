// RUN: ast -f test "%s" 2>&1 | FileCheck %s

void test() {
l:
  ;
  void *p = &&l;
  goto *p;
};

// CHECK:   CompoundStmt
// CHECK-NEXT:     LabelStmt
// CHECK-NEXT:       LabelDeclRef l
// CHECK-NEXT:       NullStmt
// CHECK-NEXT:     DeclStmt
// CHECK-NEXT:       VarDecl
// CHECK-NEXT:         DeclarationName p
// CHECK-NEXT:         PointerType
// CHECK-NEXT:           BuiltinType void
// CHECK-NEXT:         AddrLabelExpr
// CHECK-NEXT:           LabelDeclRef l
// CHECK-NEXT:     IndirectGotoStmt
// CHECK-NEXT:       ImplicitCastExpr NoOp
// CHECK-NEXT:         ImplicitCastExpr LValueToRValue
// CHECK-NEXT:           DeclRefExpr
// CHECK-NEXT:             DeclarationName p
