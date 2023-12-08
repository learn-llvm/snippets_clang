#!/usr/bin/env python

import argparse
import os
import sys


def parse_args(argv) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="c/c++ analyzer",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument("-a", "--analyze", action="store_true" , required=False, help="analyze and print")
    parser.add_argument(
        "files", metavar="FILE", nargs="+", action="store", help="FILES to be analyzed",
    )
    return parser.parse_args(argv)


def analyze_dir(fpath: str, cb):
    for subf in os.listdir(fpath):
        fullpath = os.path.join(fpath, subf)
        if os.path.isdir(fullpath):
            analyze_dir(fullpath, cb)
        elif os.path.isfile(fullpath):
            cb(fullpath)
        else:
            print(f"{fpath} not a C/C++ file or a directory", file=sys.stderr)


if __name__ == "__main__":
    parser = parse_args(sys.argv[1:])
    for file in parser.files:
        file = os.path.realpath(file)
        from .dumper import analyze_tu
        if os.path.isdir(file):
            analyze_dir(file, analyze_tu)
        elif os.path.isfile(file):
            analyze_tu(file)
