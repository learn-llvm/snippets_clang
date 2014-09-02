#ifndef COMMON_HPP
#define COMMON_HPP

#include <string>
#include <clang-c/Index.h>

#define WITH_COLOR(color, x)         \
  {                                  \
    llvm::errs().changeColor(color); \
    x;                               \
    llvm::errs().resetColor();       \
  }

std::string getStrFromCXString(CXString const &cxstring);

#endif
