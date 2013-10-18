#!/usr/bin/python3

import sys


def main():
    from clang.cindex import Index
    from pprint import pprint

    from argparse import ArgumentParser

    parser = ArgumentParser(
        description="dump TU info",
        usage="%(prog)s [options] {filename} [clang-args*]")

    parser.add_argument("--show-ids", dest='showIDs',
                        help="don't compute cursor IDs(very slow)",
                        default=False)
    parser.add_argument("--max-depth", dest='maxDepth',
                        help="limit cursor expansion to depth N",
                        metavar="N", type=int, default=None)
    opts, unknown = parser.parse_known_args(sys.argv)

    index = Index.create()
    tu = index.parse(None, args=sys.argv[1:])
    if not tu:
        parser.error("unable to load input")

    pprint(('diags', list(map(get_diag_info, tu.diagnostics))))
    pprint(('nodes', get_info(opts, tu.cursor)))


def get_diag_info(diag):
    return {
        'severity': diag.severity,
        'location': diag.location,
        'spelling': diag.spelling,
        'ranges': diag.ranges,
        'fixits': diag.fixits
    }


def get_cursor_id(opts, cursor, cursor_list=[]):
    if not opts.showIDs:
        return None

    if cursor is None:
        return None

    for i, c in enumerate(cursor_list):
        if cursor == c:
            return i
    cursor_list.append(cursor)
    return len(cursor_list) - 1


def get_info(opts, node, depth=0):
    if opts.maxDepth is not None and depth >= opts.maxDepth:
        children = None
    else:
        children = [get_info(opts, c, depth + 1) for c in node.get_children()]

    return {
        'id': get_cursor_id(opts, node),
        'kind': node.kind,
        'usr': node.get_usr(),
        'spelling': node.spelling,
        'extent.start': node.extent.start,
        'extent.end': node.extent.end,
        'is_definition': node.is_definition(),
        'definition_id': get_cursor_id(opts, node.get_definition()),
        'children': children
    }


if __name__ == '__main__':
    main()
