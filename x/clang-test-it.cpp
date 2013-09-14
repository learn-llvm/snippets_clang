#include "clang-c/Index.h"

#include <string>
#include <vector>
#include <map>
#include <iostream>

#include <cstdarg>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cstring>


#define AST_DEBUG

class AST_Buffer {
  std::string output;
  char buf[1024];

 public:
  void printf(const char *fmt, ...) {
    char *str = buf;
    int len = sizeof(buf);
    va_list args;
    va_list tmp_args;

    for (int i = 0; i < 2; ++i) {
      va_copy(tmp_args, args);
      va_start(tmp_args, fmt);
      int ret = vsnprintf(str, len, fmt, tmp_args);
      va_end(tmp_args);
      if (ret < 0) {
        std::clog << "Invalid printf() format string: `" << fmt << "', skipping"
                  << std::endl;
        break;
      }

      if (ret < len) {
        /* Success. */
        output += str;
        break;
      }

      /* Buffer is not large enought. */
      if (i != 0) {
        /* This should not happen.
         */
        std::clog << "Unexpected printf() behaviour, skipping" << std::endl;
        break;
      }
      len = ret;
      str = (char *)malloc(len);
    }
    if (str != buf) free(str);
  }

  const std::string &str() const { return output; }

  void clear() { output.clear(); }
};

class AST_Formatter {
 public:
  virtual ~AST_Formatter() {}

  virtual bool is_recursive() const = 0;

  /* Begin node formatting.
   * Process node referred by given cursor and print output to given buffer.
   *
   * If is_recursive() is true, formatters of child nodes will be invoked
   * between begin() and end().
   */
  virtual void begin(AST_Buffer &buffer, CXCursor cursor) const = 0;

  /* Finish node formatting.
   */
  virtual void end(AST_Buffer &buffer, CXCursor cursor) const = 0;
};

/* Builtin type names.
 * From here: https://github.com/stedolan/cshore/blob/master/ffigen.c
 */
static const struct {
  enum CXTypeKind kind;
  const char *name;
} ast_builtin_types[] = {
  {CXType_Void, "void"}, {CXType_Bool, "bool"}, {CXType_Char_U, "char"},
  {CXType_Char_S, "char"}, {CXType_UChar, "unsigned char"},
  {CXType_SChar, "signed char"}, {CXType_WChar, "wchar_t"},
  {CXType_Char16, "char16_t"}, {CXType_Char32, "char32_t"},
  {CXType_Short, "short"}, {CXType_UShort, "unsigned short"},
  {CXType_Int, "int"}, {CXType_UInt, "unsigned int"}, {CXType_Long, "long"},
  {CXType_ULong, "unsigned long"}, {CXType_LongLong, "unsigned long"},
  {CXType_ULongLong, "unsigned long long"}, {CXType_Int128, "__int128"},
  {CXType_UInt128, "unsigned __int128"}, {CXType_Float, "float"},
  {CXType_Double, "double"}, {CXType_LongDouble, "long double"},
};

/*! Get spelling for given cursor.
 */
static const char *ast_cursor_spelling(CXCursor cursor) {
  return clang_getCString(clang_getCursorSpelling(cursor));
}

/*! Get kind spelling for given cursor (for debug only!).
 *
 * From libclang docs:
 *     These routines are used for testing and debugging, only, and should not
 *     be relied upon.
 */
static const char *ast_kind_spelling(CXCursor cursor) {
  return clang_getCString(
      clang_getCursorKindSpelling(clang_getCursorKind(cursor)));
}

/*! Get spelling for given type.
 */
static const char *ast_type_spelling(CXType type) {
  CXCursor type_decl = clang_getTypeDeclaration(type);

  if (!clang_isInvalid(clang_getCursorKind(type_decl))) {
    return ast_cursor_spelling(type_decl);
  }

  for (int i = 0; i < sizeof(ast_builtin_types) / sizeof(ast_builtin_types[0]);
       ++i) {
    if (ast_builtin_types[i].kind == type.kind) {
      return ast_builtin_types[i].name;
    }
  }

  std::clog << "Can't retrieve type spelling (kind = " << type.kind << ")"
            << std::endl;
  return "";
}

/*! Inclusion directive formatter.
 */
class Fmt_Inclusion_Directive : public AST_Formatter {
 public:
  bool is_recursive() const { return false; }

  void begin(AST_Buffer &buffer, CXCursor cursor) const {
    const char *include_path = ast_cursor_spelling(cursor);
    buffer.printf("(\"%s\" include", include_path);
  }

  void end(AST_Buffer &buffer, CXCursor) const { buffer.printf(")"); }
};

/*! Function declaration formatter.
 */
class Fmt_Function_Decl : public AST_Formatter {
 public:
  bool is_recursive() const {
    /* Function declaration can contain child nodes for every argument.
     */
    return true;
  }

  void begin(AST_Buffer &buffer, CXCursor cursor) const {
    CXType ast_func_type = clang_getResultType(clang_getCursorType(cursor));
    const char *func_name = ast_cursor_spelling(cursor);
    const char *func_type = ast_type_spelling(ast_func_type);

    buffer.printf("(\"%s\" function (:type \"%s\"", func_name, func_type);

    /* If function has arguments, open ':arguments' section.
     *
     * Arguments will be processed as child nodes. For every argument,
    * Fmt_Parm_Decl
     * formatter will be invoked.
     *
     * After all child nodes will be processed, end() method will be called.
     */
    if (clang_Cursor_getNumArguments(cursor) > 0)
      buffer.printf(" :arguments (");
  }

  void end(AST_Buffer &buffer, CXCursor cursor) const {
    /* If function has arguments, close ':arguments' section.
     */
    if (clang_Cursor_getNumArguments(cursor) > 0) buffer.printf(")");

    buffer.printf("))");
  }
};

/*! Function agrument formatter.
 */
class Fmt_Parm_Decl : public AST_Formatter {
 public:
  bool is_recursive() const { return false; }

  void begin(AST_Buffer &buffer, CXCursor cursor) const {
    const char *arg_name = ast_cursor_spelling(cursor);
    const char *arg_type = ast_type_spelling(clang_getCursorType(cursor));
    buffer.printf("(\"%s\" variable (:type \"%s\"))", arg_name, arg_type);
  }

  void end(AST_Buffer &, CXCursor) const {}
};

/*! Struct declaration formatter.
 */
class Fmt_Struct_Decl : public AST_Formatter {
 public:
  bool is_recursive() const {
    /* Struct declaration can contain child nodes for every member.
     */
    return true;
  }

  void begin(AST_Buffer &buffer, CXCursor cursor) const {
    const char *struct_name = ast_cursor_spelling(cursor);
    buffer.printf("(\"%s\" type (:members (", struct_name);
  }

  void end(AST_Buffer &buffer, CXCursor cursor) const { buffer.printf(")))"); }
};

/*! AST Node.
 * Stores information about clang node and associated formatter.
 */
struct AST_Node {
  CXCursor cursor;
  const AST_Formatter *formatter;

  AST_Node(CXCursor cursor_, const AST_Formatter *formatter_)
      : cursor(cursor_), formatter(formatter_) {}
};

/*! AST traverser state.
 */
struct AST_Context {
  /*! Current translation unit.
   */
  CXTranslationUnit tu;

  /*! File that we're parsing now.
   *
   * Is used to determine if AST node belongs to current file or to included
   * file and so should be skipped.
   */
  const char *tu_filename;

  /*! AST node stack.
   *
   * Is used to store AST path from root node to current node.
   */
  std::vector<AST_Node> ast_stack;

  /*! Output buffer.
   */
  AST_Buffer *buffer;

  AST_Context() : tu(NULL), buffer(NULL) {}
};

/*! AST traverser.
 *
 * Implements AST traversing and output formatting.
 */
class AST_Traverser {
  /*! Map AST node type to associated formatter.
   */
  typedef std::map<CXCursorKind, AST_Formatter *> AST_Formatter_Map;

  /*! Formatters map.
   *
   * Is used to find associated formatter for AST node.
   */
  AST_Formatter_Map formatters;

  /*! Current state.
   */
  AST_Context context;

 public:
  AST_Traverser() {
    formatters[CXCursor_InclusionDirective] = new Fmt_Inclusion_Directive;
    formatters[CXCursor_FunctionDecl] = new Fmt_Function_Decl;
    formatters[CXCursor_ParmDecl] = new Fmt_Parm_Decl;
    formatters[CXCursor_StructDecl] = new Fmt_Struct_Decl;
    formatters[CXCursor_CXXMethod] = new Fmt_Function_Decl;
    formatters[CXCursor_FieldDecl] = new Fmt_Parm_Decl;
  }

  ~AST_Traverser() {
    AST_Formatter_Map::iterator it = formatters.begin();

    for (; it != formatters.end(); ++it) {
      delete it->second;
    }
  }

  /*! Traverse AST, format nodes and send output to given buffer.
   */
  void traverse(AST_Buffer &buffer, CXTranslationUnit tu,
                const char *tu_filename) {
    context.tu = tu;
    context.tu_filename = tu_filename;
    context.buffer = &buffer;

    do_traverse();
  }

 private:
  /*! Libclang callback that is invoked for every AST node.
   */
  static CXChildVisitResult ast_visitor(CXCursor cursor, CXCursor parent,
                                        CXClientData client_data);

  /*! Traversing.
   */
  void do_traverse() {
    CXCursor root = clang_getTranslationUnitCursor(context.tu);

    /* Add root node to stack.
     */
    push_cursor(root, NULL);

    /*! Clear output buffer.
     */
    context.buffer->clear();

    /*! Execute ast_visitor for every AST node.
     */
    clang_visitChildren(root, ast_visitor, this);

    /* Clear the stack and finish formatting.
     */
    pop_all_cursors();
  }

  /*! Check if we want to process given AST node.
   */
  bool is_cursor_interesting(CXCursor cursor) const {
    CXSourceLocation location = clang_getCursorLocation(cursor);
    CXFile file;
    clang_getExpansionLocation(location, &file, NULL, NULL, NULL);
    if (file) {
      if (strcmp(clang_getCString(clang_getFileName(file)),
                 context.tu_filename) ==
          0) {
        /* Process node if it is from file passed to traverse().
         */
        return true;
      }
    }
    /* Skip node otherwise.
     */
    return false;
  }

  /*! Add node cursor to stack and begin node formatting.
   *
   * Returns true if child nodes should be processed, or false
   * if they should be skipped.
   */
  bool push_cursor(CXCursor cursor, const AST_Formatter *formatter) {
    AST_Node node(cursor, formatter);
    context.ast_stack.push_back(node);
    if (node.formatter) {
      node.formatter->begin(*context.buffer, cursor);
      return node.formatter->is_recursive();
    }
    return false;
  }

  /*! Remove nodes from stack, until given cursor will be on top of the stack.
   *
   * Also finish formatting of nodes that we're removing.
   */
  void pop_cursors_until_cursor_is(CXCursor cursor) {
    while (!context.ast_stack.empty()) {
      AST_Node &node = context.ast_stack.back();
      if (clang_equalCursors(node.cursor, cursor)) break;
      if (node.formatter) node.formatter->end(*context.buffer, node.cursor);
      context.ast_stack.pop_back();
    }
  }

  /*! Remove all nodes from stack.
   *
   * Also finish formatting of nodes that we're removing.
   */
  void pop_all_cursors() {
    while (!context.ast_stack.empty()) {
      AST_Node &node = context.ast_stack.back();
      if (node.formatter) node.formatter->end(*context.buffer, node.cursor);
      context.ast_stack.pop_back();
    }
  }

  /*! Process AST node.
   *
   * Return true if child nodes should be processed too, or false
   * if they should be skipped.
   */
  bool process_ast_node(CXCursor cursor, CXCursor parent) {
    /* While previous node is not parent of new node, previous node formatting
     * is finished,
     * so we should call end() method of formatter and remove it from stack.
     */
    pop_cursors_until_cursor_is(parent);

    /* Skip node if it is not interesting enough.
     */
    if (!is_cursor_interesting(cursor)) return false;

#ifdef AST_DEBUG
    std::clog << "AST_Traverser: cxcursor: "
              << "kind = " << cursor.kind << ", kindspelling = `"
              << ast_kind_spelling(cursor) << "'"
              << ", spelling = `" << ast_cursor_spelling(cursor) << "'"
              << std::endl;
#endif
    /* Find formatter for given node.
     */
    AST_Formatter_Map::const_iterator it =
        formatters.find(clang_getCursorKind(cursor));
    if (it != formatters.end()) {
      /* Add node to stack and begin formatting.
       *
       * Formatting will be finished after all child node's will be processed.
       */
      return push_cursor(cursor, it->second);
    }

    /* Skip node if there is no formatter for it.
     */
    return false;
  }
};

CXChildVisitResult AST_Traverser::ast_visitor(CXCursor cursor, CXCursor parent,
                                              CXClientData client_data) {
  AST_Traverser *traverser = (AST_Traverser *)client_data;
  if (traverser->process_ast_node(cursor, parent)) {
    return CXChildVisit_Recurse;
  } else {
    return CXChildVisit_Continue;
  }
}

int main(int argc, char *argv[]) {
  const char *file = argv[1];
  CXIndex index = clang_createIndex(0, 0);
  int flags = CXTranslationUnit_DetailedPreprocessingRecord;
  CXTranslationUnit tu =
      clang_parseTranslationUnit(index, 0, argv, argc, 0, 0, flags);
  AST_Buffer buffer;
  AST_Traverser traverser;
  traverser.traverse(buffer, tu, file);
  printf("%s\n", buffer.str().c_str());
  clang_disposeTranslationUnit(tu);
  clang_disposeIndex(index);
  return 0;
}
