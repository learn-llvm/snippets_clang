#!/usr/bin/env python

from __future__ import print_function
import sys
import os
import subprocess
from colorama import init, Fore
import re
import argparse

NO_BUG = 'No bugs found.'


def which(program):
    def is_exe(fpath):
        if not os.path.isfile(fpath) or not os.access(fpath, os.X_OK):
            return False
        return True

    fpath, fname = os.path.split(program)
    if fpath:
        if is_exe(program):
            return program
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            path = path.strip('"')
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file
    return None


def cons(prefix, l):
    il = ['{} {}'.format(prefix, i) for i in l]
    return ' '.join(il)


def pp(hint, content):
    print(
        '{}{:20} {}{}{}'.format(
            Fore.YELLOW,
            hint,
            Fore.GREEN,
            content,
            Fore.RESET))


def get_plugin_list():
    real_file = os.path.realpath(os.path.abspath(__file__))
    curdir = os.path.dirname(real_file)
    plugin_dir = os.path.normpath(os.path.join(curdir, '../../build/bin'))
    load_list = []
    for p in os.listdir(plugin_dir):
        if p.endswith('.so'):
            full_p = os.path.join(plugin_dir, p)
            load_list.append(full_p)
            return load_list


disabled_list = ['core.DivideZero']
enabled_list = ['chx.DZChecker', 'chx.NetChecker']
load_list = get_plugin_list()
def cli_parser(args):
    parser = argparse.ArgumentParser()
    parser.add_argument('-p', nargs='+', default=load_list, help='plugins')
    parser.add_argument(
        '-d',
        nargs='+',
        default=disabled_list,
        help='disabled checkers')
    parser.add_argument(
        '-e',
        nargs='+',
        default=enabled_list,
        help='enabled checkers')
    parser.add_argument(
        'options',
        nargs='+',
        help='regular command line options')
    args = parser.parse_args(args)
    return args


def main():
    init()
    args = cli_parser(sys.argv[1:])
    exe_dict = {}
    for exe_name in ["scan-build", "scan-view", "ccc-analyzer", "clang"]:
        exe = which(exe_name)
        assert(exe)
        exe_dict[exe_name] = exe
    analyzer = '{} --use-analyzer {}'.format(
        exe_dict['scan-build'],
        exe_dict['clang'])
    cmd_str = "{} {} {} {}".format(analyzer,
                                   cons('-load-plugin', args.p),
                                   cons('-disable-checker', args.d),
                                   cons('-enable-checker', args.e))
    for k in [args.p, args.d, args.e]:
        pp('hhh', k)

    for argv in sys.argv[1:]:
        cmd_str += (" " + argv)
    print(cmd_str)

    process = subprocess.Popen(
        cmd_str.split(),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE)
    out, err = process.communicate()
    for line in out.splitlines():
        if exe_dict['ccc-analyzer'] in line:
            print(line)
    print('=' * 80)
    # report
    if NO_BUG in out:
        print(NO_BUG)
        sys.exit(0)
    view = False
    if view:
        pat = r"Run '(scan-view /tmp/scan-build-.*)' to examine bug reports"
        match = re.search(pat, out)
        assert(match)
        cmd = match.group(1)
        subprocess.call(cmd.split())
    else:
        print(err)
        print('=' * 80)

if __name__ == '__main__':
    main()
