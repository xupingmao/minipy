
# print ast

import parse
import sys


if __name__ == '__main__':
    tree = parse.parse(load(sys.argv[1]))
    parse.print_ast(tree)