#include "Common.hpp"

std::string getStrFromCXString(CXString const &cxstring) {
  std::string str;
  char const *cstr = clang_getCString(cxstring);
  if (cstr != nullptr) str = cstr;
  clang_disposeString(cxstring);
  return str;
}
