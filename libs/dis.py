import encode

compilefile = encode.compilefile
compile = encode.compile

from boot import *
from tmcode import *
import sys

global_mod_list = [LOAD_GLOBAL, STORE_GLOBAL, LOAD_CONSTANT, TM_DEF]
def dis(fname ,constants = ['None']):
    s = load(fname)
    i = 0; l = len(s)
    #constants = ['None']
    while i < l:
        op = ord(s[i])
        val = uncode16(s[i+1], s[i+2])
        i+=3
        if op in (NEW_STRING, NEW_NUMBER):
            vv = s.substring(i, i + val)
            i += val
        elif op in global_mod_list:
            print(tmcodes[op], get_const(val))
        else:
            print(tmcodes[op], val)

def dis_func(func):
    code = get_func_code(func)
    if code == None:
        print("native function")
        return
    dis_code(code)

def dissimple0(code, limit = None):
    s = code
    i = 0
    l = len(s)
    count = 0
    while i < l:
        ins = s[i]
        i+=1
        ins = ord(ins)
        val = uncode16(s[i], s[i+1])
        i+=2
        print(tmcodes[ins], val)
        count += 1
        if limit and count >= limit:
            break

def dis_code(s, start = 0, end = 0):
    i =0
    l = len(s)
    lineno = 0
    while i < l:
        op = ord(s[i])
        val = uncode16(s[i+1], s[i+2])
        i+=3
        lineno += 1
        if end and lineno > end:break
        if lineno > start:
            if op in global_mod_list:
                val = get_const(val)
            printf("%4s: %s, %s\n", lineno, tmcodes[op], val)

def dis_txt(s):
    dis_code(compile(s))

def dissimple(argv):
    argc = len(argv)
    s = argv[0]
    if s.endswith(".py"):
        s = compilefile(s)
        showconst = True
    else:
        s = load(s)
    start = 0
    count = None
    end = None
    if argc == 2: 
        start = 0
        count = int(argv[1])
        end = start + count
    if argc == 3:
        start = int(argv[1])
        count = int(argv[2])
        end = start + count
    dis_code(s, start, end)

def main():
    argc = len(ARGV)
    if argc == 2:
        fname = ARGV[1]
        dis(fname)
    elif argc == 3:
        opt = ARGV[1]
        fname = ARGV[2]
        if opt == '-src':
            tmp = compilefile(fname, "temp")
            dis("temp", get_const_list())
            rm("temp")
        elif opt == '-const':
            dis(fname, 'const')
        elif opt == '-simple':
            dissimple0(load(fname))

def copy(collection):
    vv = []
    for c in collection:
        vv.append(c)
    return vv

test = None
if __name__ == "__main__":
    argv = copy(sys.argv)
    del argv[0]
    dissimple(argv)
