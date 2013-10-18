#!/usr/bin/python3


def main():
    from clang.cindex import Index
    from pprint import pprint

    from optparse import OptionParser

    global opts

    parser = OptionParser("usage: %prog [options] {filename} [clang-args*]")
    parser.add_option("", "--show-ids", dest="showIDs",
                      help="don't compute cursor IDs(very slow)",
                      default=False)
    parser.add_option("", "--max-depth", dest="maxDepth",
                      help="limit cursor expansion to depth N",
                      metavar="N", type=int, default=None)
    parser.disable_interspersed_args()
    (opts, args) = parser.parse_args()

    if len(args) == 0:
        parser.error('invalid number arguments')

    print("type(args):", type(args), " args: ", args)

    index = Index.create()
    tu = index.parse(None, args)
    if not tu:
        parser.error("unable to load input")

    # pprint(('diags', map(get_diag_info, tu.diagnostics)))
    # pprint(('nodes', get_info(tu.cursor)))


def get_diag_info(diag):
    return {
        'severity': diag.severity,
        'location': diag.location,
        'spelling': diag.spelling,
        'ranges': diag.ranges,
        'fixits': diag.fixits
    }


def get_cursor_id(cursor, cursor_list=[]):
    if not opts.showIDs:
        return None

    if cursor is None:
        return None

    for i, c in enumerate(cursor_list):
        if cursor == c:
            return i
    cursor_list.append(cursor)
    return len(cursor_list) - 1


def get_info(node, depth=0):
    if opts.maxDepth is not None and depth >= opts.maxDepth:
        children = None
    else:
        children = [get_info(c, depth+1) for c in node.get_children()]

    return {
        'id': get_cursor_id(node),
        'kind': node.kind,
        'usr': node.get_usr(),
        'spelling': node.spelling,
        'extent.start': node.extent.start,
        'extent.end': node.extent.end,
        'is_definition': node.is_definition(),
        'definition_id': get_cursor_id(node.get_definition()),
        'children': children
    }


if __name__ == '__main__':
    main()
