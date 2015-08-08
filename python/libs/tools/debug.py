
from boot import *

def _debug(*vars):
    print(vars)
    input()

def debug_line(line):
    left = ''
    rest = ''
    state = 0
    for c in line:
        if state == 1:
            rest += c
        elif c == ' ' or c == '\t':
            left += c
        else:
            state = 1
    return left + "_debug();" + rest

def _insert_debug(fname, line_nums):
    content = load(fname)
    content = content.replace("\r", "")
    lines = content.split("\n")
    for num in line_nums:
        lines[num] = debug_line(lines[num])
    code = "\n".join(lines)
    save(fname + ".d", code)

if __name__ == "__main__":
    fname = ARGV[1]
    line_nums = []
    i = 2
    while i < len(ARGV):
        line_nums.append(int(ARGV[i]))
        i+=1
    _insert_debug(ARGV[1],line_nums)
