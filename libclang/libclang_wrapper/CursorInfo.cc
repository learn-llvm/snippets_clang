#include "CursorInfo.hpp"
#include "CursorCounter.hpp"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

LocPoint LocPoint::getLocPoint(
    CXSourceLocation location,
    std::function<void(CXSourceLocation, CXFile *, unsigned *, unsigned *,
                       unsigned *)> genenerator) {
  CXFile file;
  unsigned line, column, offset;
  genenerator(location, &file, &line, &column, &offset);
  CXString fileCXStr = clang_getFileName(file);
  return LocPoint(getStrFromCXString(fileCXStr), line, column, offset);
}

LocPoint LocPoint::getPresumedPoint(CXSourceLocation location) {
  CXString fileCXStr;
  unsigned line, column;
  unsigned offset = 0;
  clang_getPresumedLocation(location, &fileCXStr, &line, &column);
  LocPoint presumed(getStrFromCXString(fileCXStr), line, column, offset);
  clang_disposeString(fileCXStr);
  return presumed;
}

void LocPoint::dump() {
  errs() << filename_ << " [" << line_ << ", " << column_ << "]\n";
}

CursorInfo CursorInfo::getCursorInfo(CXCursor cursor) {
  std::string &&cursorStr = getStrFromCXString(clang_getCursorSpelling(cursor));
  CXSourceLocation location = clang_getCursorLocation(cursor);
  LocPoint &&expandPoint =
      LocPoint::getLocPoint(location, clang_getExpansionLocation);
  LocPoint &&presumedPoint = LocPoint::getPresumedPoint(location);
  LocPoint &&instantiationPoint =
      LocPoint::getLocPoint(location, clang_getInstantiationLocation);
  LocPoint &&spellingPoint =
      LocPoint::getLocPoint(location, clang_getSpellingLocation);
  expandPoint.dump();
  presumedPoint.dump();
  instantiationPoint.dump();
  spellingPoint.dump();
  return CursorInfo(cursorStr, expandPoint, presumedPoint, instantiationPoint);
}
