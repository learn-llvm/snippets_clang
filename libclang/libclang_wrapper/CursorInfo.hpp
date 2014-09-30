#ifndef CURSORINFO_HPP
#define CURSORINFO_HPP

#include <functional>

#include "Common.hpp"

struct LocPoint {
  std::string filename_;
  unsigned line_, column_, offset_;

  template <typename FnTy>
  static LocPoint getLocPoint(CXSourceLocation location,
                              /*std::function<void(CXSourceLocation, CXFile *,
                                unsigned *, unsigned *,
                                unsigned *)>*/
                              FnTy genenerator);
  static LocPoint getPresumedPoint(CXSourceLocation location);
  LocPoint() = default;
  LocPoint(std::string &&filename, unsigned line, unsigned column,
           unsigned offset = 0)
      : filename_(std::move(filename)),
        line_(line),
        column_(column),
        offset_(offset) {}
  LocPoint &operator=(LocPoint const &) = delete;
};

class CursorInfo {
  CXCursor cursor_;
  std::string cursorStr_;
  LocPoint expand_, presumed_, instantantiation_;
  CXFile included_;
  CXCursor lexParent_, semaParent_, parent_;
  CXLinkageKind linkage_;
  CXCursorKind kind_;

 public:
  static CursorInfo getCursorInfo(CXCursor cursor);
  CursorInfo(CXCursor &&cursor, LocPoint &&expand, LocPoint &&presumed,
             LocPoint &&instantantiation)
      : cursor_(cursor),
        expand_(std::move(expand)),
        presumed_(std::move(presumed)),
        instantantiation_(std::move(instantantiation)) {
    init();
  }
  CursorInfo &operator=(CursorInfo const &) = delete;
  void init();
  void dump();
};

#endif
