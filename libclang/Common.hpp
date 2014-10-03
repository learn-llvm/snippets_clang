#ifndef COMMON_HPP
#define COMMON_HPP

#include <string>
#include "clang-c/Index.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Format.h"

using namespace llvm;

#define WITH_COLOR(color, x)         \
  {                                  \
    llvm::errs().changeColor(color); \
    x;                               \
    llvm::errs().resetColor();       \
  }

std::string getStrFromCXString(CXString const &cxstring);

#endif
