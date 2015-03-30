#!/usr/bin/env python

from __future__ import print_function

import argparse
import sys
import subprocess

my_options = '-triple x86_64-unknown-linux-gnu -E -disable-free -main-file-name null -mrelocation-model static -mthread-model posix -mdisable-fp-elim -fmath-errno -masm-verbose -mconstructor-aliases -munwind-tables -fuse-init-array -target-cpu x86-64 -v -dwarf-column-info -resource-dir /home/hongxu/RESEARCH/llvm-git/obj/bin/../lib/clang/3.7.0 -c-isystem /home/hongxu/RESEARCH/klee/src/include -c-isystem /home/hongxu/RESEARCH/klee/src/include/../../obj/include -c-isystem /usr/lib/odb/x86_64-linux-gnu/include -c-isystem /home/hongxu/RESEARCH/llvm-git/src/include -c-isystem /home/hongxu/qemu/panda/distorm/include -c-isystem /home/hongxu/.linuxbrew/include -c-isystem . -cxx-isystem /home/hongxu/RESEARCH/llvm-git/src/tools/clang/include -cxx-isystem /home/hongxu/RESEARCH/llvm-git/obj/tools/clang/include -cxx-isystem /home/hongxu/RESEARCH/klee/src/include -cxx-isystem /home/hongxu/RESEARCH/klee/src/include/../../obj/include -cxx-isystem /usr/lib/odb/x86_64-linux-gnu/include -cxx-isystem /home/hongxu/RESEARCH/llvm-git/src/include -cxx-isystem /home/hongxu/qemu/panda/distorm/include -cxx-isystem /home/hongxu/.linuxbrew/include -cxx-isystem . -internal-isystem /usr/local/include -internal-isystem /home/hongxu/RESEARCH/llvm-git/obj/bin/../lib/clang/3.7.0/include -internal-externc-isystem /usr/include/x86_64-linux-gnu -internal-externc-isystem /include -internal-externc-isystem /usr/include -fdebug-compilation-dir /home/hongxu/src/snippets_clang/build/bin -ferror-limit 19 -fmessage-length 191 -mstackrealign -fobjc-runtime=gcc -fdiagnostics-show-option -fcolor-diagnostics -o - -x c /dev/null'


def cli_parser(args):
    parser = argparse.ArgumentParser(description='utils for clang')
    parser.add_argument(
        '-l',
        '--load',
        nargs='+',
        required=False,
        help='additional loaded plugins')
    parser.add_argument('-f', '--file', required=False)
    parser.add_argument('-c', '--checkers', action='store_true')
    args = parser.parse_args(args)
    return args


def main():
    args = cli_parser(sys.argv[1:])
    load_so = []
    if args.load:
        for l in args.load:
            load_so.append('-load')
            load_so.append(l)
    if args.checkers:
        cmd = 'clang -cc1 {} -analyzer-checker-help'.format(my_options)
        print(cmd)
        subprocess.call(cmd.split())
        sys.exit(0)
    cmd = 'clang -cc1 {} -analyze {} {}'.format(my_options, ' '.join(load_so), args.file)
    print(cmd)
    subprocess.call(cmd.split())

if __name__ == '__main__':
    main()
