#include <clang-c/Index.h>

enum CXChildVisitResult visitor(CXCursor cursor, CXCursor parent, CXClientData data)
{
  return CXChildVisit_Continue;
}

int main(int argc, char * argv[])
{
  CXIndex index = clang_createIndex(0, 0);
  const char* const* args = (const char* const*)argv;
  CXTranslationUnit txUnit = clang_parseTranslationUnit(index, 0, args, argc, 0, 0, CXTranslationUnit_None);

  CXCursor cur = clang_getTranslationUnitCursor(txUnit);
  clang_visitChildren(cur, visitor, NULL);

  clang_disposeTranslationUnit(txUnit);
  clang_disposeIndex(index);
  return 0;
}
