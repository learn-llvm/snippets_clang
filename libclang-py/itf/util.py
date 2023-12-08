import os
from enum import Enum

from clang.cindex import SourceLocation


class Node:
    def __init__(self, node_id: int, fqn: str, location: SourceLocation) -> None:
        self.id: int = node_id
        self.fqn: str = fqn
        self.location: SourceLocation = location

    def __str__(self) -> str:
        return self.fqn

    def __repr__(self) -> str:
        return f"{self.id}: {self.fqn}, {self.location}"

def loc_str(location) -> str:
    if isinstance(location, SourceLocation):
        sloc: SourceLocation = location
        return f"{sloc.file}:{sloc.line}:{sloc.column}"
    raise RuntimeError(f"{location}")

class FileKind(Enum):
    CC = 1
    HH = 2
    NN = 3


hh_suffixes = {".h", ".hh", ".hpp", ".hxx"}
cc_suffixes = {".c", ".cc", ".cpp", ".cxx"}

def get_kind_from_filename(fpath: str) -> FileKind:
    suffix = os.path.splitext(fpath)[-1]
    if suffix in cc_suffixes:
        return FileKind.CC
    elif suffix in hh_suffixes:
        return FileKind.HH
    return FileKind.NN
