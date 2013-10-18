#!/usr/bin/python3


def main():
    import sys
    from clang.cindex import Index

    from argparse import ArgumentParser

    parser = ArgumentParser(
        prog=sys.argv[0],
        description="print header dependencies",
        version="1.0.0",
        usage="%(prog)s {filename} [clang-args*]")

    if(len(sys.argv) <= 1):
        parser.print_help()
        exit(1)

    out = sys.stdout

    index = Index.create()
    tu = index.parse(None, sys.argv[1:])
    if not tu:
        parser.error("unable to load input")

    def name(f):
        if f:
            return "\"" + f.name.decode('utf-8') + "\""

    out.write("digraph G {\n")
    for i in tu.get_includes():
        line = "   "
        if i.is_input_file:
            line += name(i.include)
        else:
            line += '%s->%s' % (name(i.source), name(i.include))
        line += "\n"
        out.write(line)
    out.write("}\n")

if __name__ == '__main__':
    main()
