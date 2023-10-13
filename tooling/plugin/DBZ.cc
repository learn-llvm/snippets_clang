#include "clang/StaticAnalyzer/Core/Checker.h"
#include "clang/StaticAnalyzer/Core/BugReporter/BugType.h"
#include "clang/StaticAnalyzer/Core/CheckerRegistry.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CheckerContext.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ConstraintManager.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CallEvent.h"
#include <iostream>

using namespace clang;
using namespace ento;

namespace chx {
class MyDZChecker
    : public Checker<check::PreStmt<BinaryOperator>, check::PostCall> {
  mutable std::unique_ptr<BugType> BT;
  void reportBug(char const *Msg, ProgramStateRef StateZero,
                 CheckerContext &C) const;

 public:
  MyDZChecker()
      : BT(std::make_unique<BugType>(this, "chx-DBZ", categories::LogicError)) {
  }
  void checkPreStmt(BinaryOperator const *B, CheckerContext &C) const;
  void checkPostCall(CallEvent const &, CheckerContext &) const;
};

void MyDZChecker::reportBug(const char *Msg, ProgramStateRef StateZero,
                            CheckerContext &C) const {
  if (ExplodedNode *N = C.generateSink(StateZero)) {
    BugReport *R = new BugReport(*BT, Msg, N);
    // bugreporter::trackNullOrUndefValue(N, bugreporter::GetDenomExpr(N), *R);
    C.emitReport(R);
  }
}

void MyDZChecker::checkPreStmt(const BinaryOperator *B,
                               CheckerContext &C) const {
  BinaryOperator::Opcode Op = B->getOpcode();
  if (Op != BinaryOperatorKind::BO_Div && Op != BinaryOperatorKind::BO_Rem &&
      Op != BinaryOperatorKind::BO_DivAssign &&
      Op != BinaryOperatorKind::BO_RemAssign)
    return;

  if (!B->getRHS()->getType()->isScalarType()) return;

  SVal Denom = C.getState()->getSVal(B->getRHS(), C.getLocationContext());
  SVal Numer = C.getState()->getSVal(B->getLHS(), C.getLocationContext());
  Optional<DefinedSVal> DVR = Denom.getAs<DefinedSVal>();
  Optional<DefinedSVal> DVL = Numer.getAs<DefinedSVal>();

  if (!DVR) return;

  ConstraintManager &CM = C.getConstraintManager();
  ProgramStateRef stateNotZero, stateZero;
  std::tie(stateNotZero, stateZero) = CM.assumeDual(C.getState(), *DVR);

  if (stateNotZero != nullptr) {
    stateNotZero->dump();
  }
  if (stateZero != nullptr) {
    stateZero->dump();
  }

  // surely 0
  if (stateNotZero == nullptr) {
    assert(stateZero);
    reportBug("chx.DBZ - DBZ", stateZero, C);
    return;
  }

  /// std::cerr << (stateNotZero != nullptr) << "  " << (stateZero != nullptr)
  ///           << '\n';

  bool TaintedD = C.getState()->isTainted(*DVR);
  if ((stateNotZero != nullptr && stateZero != nullptr && TaintedD)) {
    reportBug("chx.DBZ - tainted DBZ", stateZero, C);
    return;
  }

  // If we get here, then the denom should not be zero. We abandon the implicit
  // zero denom case for now.
  C.addTransition(stateNotZero);
}

void MyDZChecker::checkPostCall(CallEvent const &call,
                                CheckerContext &ctx) const {
  IdentifierInfo const *ID = call.getCalleeIdentifier();
  if (ID == nullptr) return;
  if (ID->getName() == "rand") {
    ProgramStateRef state = ctx.getState();
    SymbolRef sym = call.getReturnValue().getAsSymbol();
    if (sym) {
      ProgramStateRef newState = state->addTaint(sym);
      ctx.addTransition(newState);
    }
  }
}

}  // end namespace
