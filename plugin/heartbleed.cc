#include "clang/StaticAnalyzer/Core/Checker.h"
#include "clang/StaticAnalyzer/Core/BugReporter/BugType.h"
#include "clang/StaticAnalyzer/Core/CheckerRegistry.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CheckerContext.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/ConstraintManager.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CallEvent.h"

#include "llvm/Support/raw_ostream.h"

#include <iostream>

/// http://blog.trailofbits.com/2014/04/27/using-static-analysis-and-clang-to-find-heartbleed/

using namespace clang;
using namespace ento;

namespace chx {

// check::ASTDecl,check::PreStmt, check::PostStmt, check::Event,
class NetworkTaintChecker
    : public Checker<check::PreCall, check::PostCall, check::Location,
                     check::ASTCodeBody, check::EndOfTranslationUnit,
                     check::Bind, check::EndAnalysis, check::EndFunction,
                     check::BranchCondition, check::DeadSymbols
                     // check::RegionChanges,
                     // check::PointerEscape,
                     // check::ConstPointerEscape
                     > {
  mutable std::unique_ptr<BugType> BT;

  bool isArgUnConstrained(llvm::Optional<NonLoc>, SValBuilder &,
                          ProgramStateRef) const;

 public:
  NetworkTaintChecker(void)
      : BT(std::make_unique<BugType>(this, "My Tainted dereference",
                                     "MY Custom Analyzer")) {}

  void checkPreCall(const CallEvent &, CheckerContext &) const;
  void checkPostCall(const CallEvent &, CheckerContext &) const;
  void checkLocation(SVal, bool, const Stmt *, CheckerContext &) const;
  // unimplemented
  void checkASTCodeBody(const Decl *D, AnalysisManager &mgr,
                        BugReporter &BR) const {}

  void checkEndOfTranslationUnit(const TranslationUnitDecl *TU,
                                 AnalysisManager &mgr, BugReporter &BR) const {}
  void checkBind(const SVal &location, const SVal &val, const Stmt *S,
                 CheckerContext &C) const {}
  void checkEndAnalysis(ExplodedGraph &G, BugReporter &BR,
                        ExprEngine &Eng) const {}
  void checkEndFunction(CheckerContext &C) const {}
  void checkBranchCondition(const Stmt *Condition, CheckerContext &C) const {}
  void checkDeadSymbols(SymbolReaper &SR, CheckerContext &C) const {}

  // bool wantsRegionChangeUpdate(ProgramStateRef state) const { return false; }
  // ProgramStateRef checkRegionChanges(ProgramStateRef state,
  //                                    const InvalidatedSymbols *invalidated,
  //                                    ArrayRef<const MemRegion *> Explicits,
  //                                    ArrayRef<const MemRegion *> Regions,
  //                                    const CallEvent *Call) const {
  //   return nullptr;
  // }
  // ProgramStateRef checkPointerEscape(
  //     ProgramStateRef State, const InvalidatedSymbols &Escaped,
  //     const CallEvent *Call, PointerEscapeKind Kind,
  //     RegionAndSymbolInvalidationTraits *ETraits) const {
  //   return nullptr;
  // }
  // ProgramStateRef checkConstPointerEscape(
  //     ProgramStateRef State, const InvalidatedSymbols &Escaped,
  //     const CallEvent *Call, PointerEscapeKind Kind,
  //     RegionAndSymbolInvalidationTraits *ETraits) const {
  //   return nullptr;
  // }
};

// checker logic
bool NetworkTaintChecker::isArgUnConstrained(Optional<NonLoc> Arg,
                                             SValBuilder &builder,
                                             ProgramStateRef state) const {
  bool result = false;

  if (Arg) {
    // so 5000 is chosen as an arbitrary value. reall what we should do is
    // compare the range on the value with the range of the memory object
    // pointed to by either the base pointer, in an array dereference, or the
    // first and second parameters to memcpy, in a call to memcpy. however,
    // frequently this information is opaque to the analyzer. what I mostly
    // wanted to answer was, show me locations in the code where NO constraints,
    // practically, had been applied to the size. this would still permit
    // technically incorrect constraints to be passed, so there is room for
    // improvement, but I think that generally, something sound is unattainable
    // here so we just do what we can in the time allotted
    llvm::APInt V(32, 5000);
    SVal Val = builder.makeIntVal(V, false);

    Optional<NonLoc> NLVal = Val.getAs<NonLoc>();

    if (NLVal.hasValue() == false) return result;

    SVal cmprLT = builder.evalBinOpNN(state, BinaryOperatorKind::BO_GT, *Arg,
                                      *NLVal, builder.getConditionType());

    Optional<NonLoc> NLcmprLT = cmprLT.getAs<NonLoc>();

    if (NLcmprLT.hasValue() == false) return result;

    std::pair<ProgramStateRef, ProgramStateRef> p = state->assume(*NLcmprLT);

    ProgramStateRef trueState = p.first;

    if (trueState) result = true;
  }

  return result;
}

// check memcpy / memset calls
void NetworkTaintChecker::checkPreCall(const CallEvent &call,
                                       CheckerContext &C) const {
  // ProgramStateRef state = C.getState();
  const IdentifierInfo *ID = call.getCalleeIdentifier();
  std::cerr << " [checkPreCall]: " << ID->getName().data() << "\n";

  if (ID == nullptr) {
    return;
  }

  if (ID->getName() == "memcpy") {
    // check if the 3rd argument is tainted and constrained
    SVal SizeArg = call.getArgSVal(2);
    ProgramStateRef state = C.getState();

    if (state->isTainted(SizeArg)) {
      SValBuilder &svalBuilder = C.getSValBuilder();
      Optional<NonLoc> SizeArgNL = SizeArg.getAs<NonLoc>();

      if (isArgUnConstrained(SizeArgNL, svalBuilder, state) == true) {
        ExplodedNode *loc = C.generateSink();
        if (loc) {
          BugReport *bug = new BugReport(
              *this->BT, "Tainted, unconstrained value used in memcpy size",
              loc);
          C.emitReport(bug);
        }
      }
    }
  }

  return;
}

// also check address arithmetic
void NetworkTaintChecker::checkLocation(SVal l, bool isLoad, const Stmt *LoadS,
                                        CheckerContext &C) const {
  const MemRegion *R = l.getAsRegion();
  if (!R) {
    return;
  }

  const ElementRegion *ER = dyn_cast<ElementRegion>(R);
  if (!ER) {
    return;
  }

  DefinedOrUnknownSVal Idx = ER->getIndex().castAs<DefinedOrUnknownSVal>();
  ProgramStateRef state = C.getState();

  if (state->isTainted(Idx)) {
    SValBuilder &svalBuilder = C.getSValBuilder();

    Optional<NonLoc> idxNL = Idx.getAs<NonLoc>();

    if (this->isArgUnConstrained(idxNL, svalBuilder, state) == true) {
      // report a bug!
      ExplodedNode *loc = C.generateSink();
      if (loc) {
        BugReport *bug = new BugReport(
            *this->BT, "Tainted, unconstrained value used in array index", loc);
        C.emitReport(bug);
      }
    }
  }

  return;
}

// check for htonl/htons
void NetworkTaintChecker::checkPostCall(const CallEvent &Call,
                                        CheckerContext &C) const {
  const IdentifierInfo *ID = Call.getCalleeIdentifier();

  if (ID == nullptr) {
    return;
  }

  if (ID->getName() == "ntohl" || ID->getName() == "ntohs") {
    ProgramStateRef State = C.getState();
    // taint the value written to by this call
    SymbolRef Sym = Call.getReturnValue().getAsSymbol();

    if (Sym) {
      ProgramStateRef newState = State->addTaint(Sym);
      C.addTransition(newState);
    }
  }

  return;
}
}  // end namespace
