/** @file */
/*
 * @brief Logical Lines Of Code Counter
 *
 */

// TODO:
// - getopt
// - categories
// - multiple files
// - summary

#include <cstdlib>
#include <iostream>
#include <string>

#include "stats.hpp"

std::map<int, Stats *> stats;

void printDecl(CXCursor c) {
  CXString n = clang_getCursorSpelling(c);
  const char *name = clang_getCString(n);
  CXString cx_objc_type = clang_getDeclObjCTypeEncoding(c);
  const char *type = clang_getCString(cx_objc_type);
  if (!name) name = "Unknown";
  if (!type) type = "Unknown";
  std::cout << name << ": " << type << std::endl;
  clang_disposeString(n);
  clang_disposeString(cx_objc_type);
}

std::string getFileName(CXCursor cursor) {
  CXFile file;
  clang_getSpellingLocation(clang_getCursorLocation(cursor), &file, 0, 0, 0);
  CXString fileStr = clang_getFileName(file);
  const char *t = clang_getCString(fileStr);
  std::string fileName;
  if (t) fileName = t;
  clang_disposeString(fileStr);
  return fileName;
}

const char *filename;

CXChildVisitResult statsVisitor(CXCursor cursor, CXCursor parent,
                                CXClientData client_data) {
  CXSourceRange range = clang_getCursorExtent(cursor);
  std::string file = getFileName(cursor);

  //FIXME: verify kind exists in stats
  if (file == filename) {
    stats[cursor.kind]->count++;
    stats[stats[cursor.kind]->category]->count++;
    //std::cout << file << std::endl;
    printDecl( cursor );
  }
  return CXChildVisit_Recurse;
}

int main(int argc, char **argv) {
  Stats::Init(stats);

  if (argc < 2) {
    std::cout <<"usage:" << argv[0] << " file.cc" << std::endl;
    return 1;
  }
  filename = argv[1];

  CXIndex idx = clang_createIndex(1, 0);

  CXTranslationUnit TU = clang_parseTranslationUnit(
      idx, filename, argv + 2, argc - 2, 0, 0,
      CXTranslationUnit_PrecompiledPreamble |
          CXTranslationUnit_PrecompiledPreamble |
          CXTranslationUnit_DetailedPreprocessingRecord);

  clang_reparseTranslationUnit(TU, 0, 0, 0);

  clang_visitChildren(clang_getTranslationUnitCursor(TU), statsVisitor, NULL);

  for (std::map<int, Stats *>::iterator it = stats.begin(); it != stats.end();
       it++) {
    if (it->second->name == NULL || it->second->count == 0){
      if(it->second->name == NULL){
        std::cout<<"odd\n";
      }
      continue;
    } 
    if (it->first == Stats::Ty_Misc) std::cout << std::endl;
    std::cout << it->second->name << ": " << it->second->count << std::endl;
  }
  std::cout << std::endl;
  std::cout
      << "Total LLOC: "
      << (stats[Stats::Ty_Declaration]->count +
          stats[Stats::Ty_Statement]->count + stats[Stats::Ty_Loop]->count +
          stats[Stats::Ty_Condition]->count +
          stats[Stats::Ty_Function]->count + stats[Stats::Ty_Call]->count)
      << std::endl;

  return 0;
}
