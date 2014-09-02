#ifndef CURSORINFO_HPP
#define CURSORINFO_HPP

#include <functional>

#include "Common.hpp"

class LocPoint {
  std::string filename_;
  unsigned line_, column_, offset_;

 public:
  static LocPoint getLocPoint(
      CXSourceLocation location,
      std::function<void(CXSourceLocation, CXFile *, unsigned *, unsigned *,
                         unsigned *)> genenerator);
  static LocPoint getPresumedPoint(CXSourceLocation location);
  LocPoint(std::string &&filename, unsigned line, unsigned column,
           unsigned offset = 0)
      : filename_(std::move(filename)),
        line_(line),
        column_(column),
        offset_(offset) {}
  void dump();
  LocPoint &operator=(LocPoint const &) = delete;
};

class CursorInfo {
  std::string cursorStr_;
  LocPoint expand_, presumed_, instantantiation_;

 public:
  static CursorInfo getCursorInfo(CXCursor cursor);
  CursorInfo(std::string const &cursorStr, LocPoint const &expand,
             LocPoint const &presumed, LocPoint const &instantantiation)
      : cursorStr_(std::move(cursorStr)),
        expand_(std::move(expand)),
        presumed_(std::move(presumed)),
        instantantiation_(std::move(instantantiation)) {}
  CursorInfo &operator=(CursorInfo const &) = delete;
  void dump();
};

#endif
