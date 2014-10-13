#include "CursorInfo.hpp"
#include "CursorCounter.hpp"

#include "Common.hpp"
#include <map>

using namespace llvm;

namespace {
#define LinkPair(name) \
  { CXLinkage_##name, #name }
std::string getLinkageName(CXCursor const &cursor) {
  typedef std::map<CXLinkageKind, std::string> LinkNameTy;
  CXLinkageKind linkage = clang_getCursorLinkage(cursor);
  static LinkNameTy linkName{LinkPair(Invalid), LinkPair(NoLinkage),
                             LinkPair(Internal), LinkPair(UniqueExternal),
                             LinkPair(External)};
  auto find = linkName.find(linkage);
  assert(find != linkName.end());
  return find->second;
}
#undef LinkPair

raw_ostream &operator<<(raw_ostream &os, LocPoint const &point) {
  os << point.filename_ << " [" << point.line_ << ", " << point.column_ << "]";
  return os;
}
}

template <typename FnTy>
LocPoint LocPoint::getLocPoint(CXSourceLocation location,
                               /*std::function<void(CXSourceLocation, CXFile *,
                                 unsigned *, unsigned *,
                                 unsigned *)>*/
                               FnTy genenerator) {
  CXFile file;
  unsigned line, column, offset;
  genenerator(location, &file, &line, &column, &offset);
  if (file == nullptr) {
    return LocPoint();
  } else {
    CXString fileCXStr = clang_getFileName(file);
    return LocPoint(getStrFromCXString(fileCXStr), line, column, offset);
  }
}

LocPoint LocPoint::getPresumedPoint(CXSourceLocation location) {
  CXString fileCXStr;
  unsigned line, column;
  unsigned offset = 0;
  // it is guaranteed that fileCXStr not null
  clang_getPresumedLocation(location, &fileCXStr, &line, &column);
  LocPoint presumed(getStrFromCXString(fileCXStr), line, column, offset);
  return presumed;
}

void CursorInfo::dump() {
  auto dumpIfLocExist = [](char const *hint, LocPoint &point) {
    if (!point.filename_.empty()) {
      errs() << hint << ": " << point << "\n";
    }
  };
  auto dumpIfNotNullCXStr = [](char const *hint, CXString const &str) {
    if (std::strlen((char const *)(str.data)) != 0) {
      errs() << hint << ": " << getStrFromCXString(str) << "\n";
    }
  };

  errs() << "cursor String: "
         << getStrFromCXString(clang_getCursorSpelling(cursor_)) << '\n';
  errs() << "cursor DisplayName: "
         << getStrFromCXString(clang_getCursorDisplayName(cursor_)) << '\n';
  dumpIfNotNullCXStr("cursor USR", clang_getCursorUSR(cursor_));
  if (clang_isPreprocessing(kind_)) {
    if (expand_.filename_.empty()) {
      return;
    }
  }

  errs() << "linkage: " << getLinkageName(cursor_) << '\n';
  errs() << "cursorKind: "
         << getStrFromCXString(clang_getCursorKindSpelling(kind_)) << '\n';
  CXType type = clang_getCursorType(cursor_);
  errs() << "typeKind: "
         << getStrFromCXString(clang_getTypeKindSpelling(type.kind)) << '\n';

  errs() << "type: " << getStrFromCXString(clang_getTypeSpelling(type)) << '\n';
  if (included_ != nullptr) {
    errs() << "included : " << getStrFromCXString(clang_getFileName(included_));
  }

  dumpIfLocExist("presumedPoint", presumed_);
  dumpIfLocExist("expandPoint", expand_);
  dumpIfLocExist("spellingPoint", spelling_);

  dumpIfNotNullCXStr("semParent", clang_getCursorSpelling(semaParent_));
  dumpIfNotNullCXStr("lexParent", clang_getCursorSpelling(lexParent_));

  auto rawCommentCXStr = clang_Cursor_getRawCommentText(cursor_);
  if (rawCommentCXStr.data != nullptr) {
    errs() << "raw: " << getStrFromCXString(rawCommentCXStr) << '\n';
    auto briefCommentCXStr = clang_Cursor_getBriefCommentText(cursor_);
    dumpIfNotNullCXStr("brief", briefCommentCXStr);
    auto comment = clang_Cursor_getParsedComment(cursor_);
    auto dumpFormattedComment = [&comment](auto &hint, auto generator) {
      auto formatCXStr = generator(comment);
      if (formatCXStr.data != nullptr) {
        errs() << hint << ": " << getStrFromCXString(formatCXStr);
      }
    };
    dumpFormattedComment("XML", clang_FullComment_getAsXML);
    dumpFormattedComment("HTML", clang_FullComment_getAsHTML);
  }
}

void CursorInfo::init() {
  included_ = clang_getIncludedFile(cursor_);
  semaParent_ = clang_getCursorSemanticParent(cursor_);
  lexParent_ = clang_getCursorLexicalParent(cursor_);
  linkage_ = clang_getCursorLinkage(cursor_);
  kind_ = clang_getCursorKind(cursor_);
}

CursorInfo CursorInfo::getCursorInfo(CXCursor cursor) {
  CXSourceLocation location = clang_getCursorLocation(cursor);
  LocPoint expandPoint =
      LocPoint::getLocPoint(location, clang_getExpansionLocation);
  LocPoint presumedPoint = LocPoint::getPresumedPoint(location);
  LocPoint spellingPoint =
      LocPoint::getLocPoint(location, clang_getSpellingLocation);
  return CursorInfo(std::move(cursor), std::move(presumedPoint),
                    std::move(expandPoint), std::move(spellingPoint));
}
