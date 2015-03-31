#include "DBZ.cc"
#include "heartbleed.cc"

extern "C" void clang_registerCheckers(CheckerRegistry &registry) {
  registry.addChecker<chx::MyDZChecker>("chx.DZChecker", "DZChecker");
  registry.addChecker<chx::NetworkTaintChecker>("chx.NetChecker", "NetChecker");
}

extern "C" const char clang_analyzerAPIVersionString[] =
    CLANG_ANALYZER_API_VERSION_STRING;
