#ifndef CURSORCOUNTER_HPP
#define CURSORCOUNTER_HPP

#include "Common.hpp"
#include <map>

class CursorCounter {
  std::string filename_;
  typedef std::map<CXCursorKind, unsigned> CursorCounterMap;
  CursorCounterMap cursor2counter_;
  CursorCounter() {}
  CursorCounter(CursorCounter const&) = delete;
  CursorCounter& operator=(CursorCounter const&) = delete;

 public:
  static CursorCounter& getInstance(void) {
    static CursorCounter counter;
    return counter;
  }
  bool insert(CXCursorKind kind);
  static std::string getCursorFileName(CXCursor cursor);
  void visit(CXTranslationUnit const& TU);
  void dump();
};

#endif
