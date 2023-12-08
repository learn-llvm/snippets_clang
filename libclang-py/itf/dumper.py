from clang.cindex import Cursor, CursorKind, Index, SourceLocation

from .util import FileKind, get_kind_from_filename, loc_str

def dump_all_ast(cursor: Cursor, i: int):
    print(" " * i, cursor.kind, ":", cursor.spelling, "(", cursor.location, ")")
    for child in cursor.get_children():
        dump_all_ast(child, i + 1)


def print_func(cursor: Cursor):
    cursor_kind: CursorKind = cursor.kind
    if cursor_kind is CursorKind.FUNCTION_DECL:
        print(f"{cursor.spelling}: {cursor_kind}, ({loc_str(cursor.location)})")
    if cursor_kind is CursorKind.VAR_DECL:
        print(f"{cursor.spelling}: {cursor_kind}, ({loc_str(cursor.location)})")
    for child in cursor.get_children():
        print_func(child)


def analyze_tu(fpath: str):
    file_kind = get_kind_from_filename(fpath)
    if file_kind == FileKind.NN:
        print(f"{fpath} not a C/C++ source file")
        return
    tu = Index.create().parse(fpath)
    print(f"=== {fpath} ===")
    # dump_all_ast(tu.cursor, 0)
    print_func(tu.cursor)

