from encode import compile, split_instr
from tmcode import *
    
def dump(text):
    code = compile(text)
    list = split_instr(code)
    s = ""
    const_list = getconst_list()
    for i in list:
        op, val = i
        s += chr(op)
        if op == LOAD_CONSTANT:
            c = const_list[val]
            if istype(c, 'string'):
                v = chr(NEW_STRING) + code16(len(c)) + c
            elif istype(c, "number"):
                c = str(c)
                v = chr(NEW_NUMBER) + code16(len(c)) + c
            else:
                raise("unknown constant type")
        elif op in [STORE_GLOBAL, LOAD_GLOBAL, TM_DEF]:
            name = const_list[op]
            v = code16(len(name)) + name
        else:
            v = code16(val)
        s += v
    return s

if __name__ == "__main__":
    if len(ARGV) < 2:
        raise("usage: %s file".format(ARGV[0]))
    fname = ARGV[1]
    code = dump(load(fname))
    save(fname + ".dump", code)