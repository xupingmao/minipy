from encode import *

def _op_add(a,b):
    return a+b
def _op_sub(a,b):
    return a-b
def _op_div(a,b):
    return a/b
def _op_mul(a,b):
    return a*b
def _op_mod(a,b):
    return a%b
def _op_get(a,b):
    return a[b]
def _op_lt(a,b):
    return a < b
def _op_gt(a,b):
    return a > b
def _op_lteq(a,b):
    return a<=b
def _op_gteq(a,b):
    return a>=b
def _op_eq(a,b):
    return a==b
def _op_ne(a,b):
    return a!=b
def _op_in(a,b):
    return a in b
def _op_del(a,b):
    del a[b]
op_dict = {
    ADD: _op_add,
    SUB: _op_sub,
    DIV: _op_div,
    MUL: _op_mul,
    MOD: _op_mod,
    GET: _op_get,
    LT: _op_lt,
    GT: _op_gt,
    LTEQ: _op_lteq,
    GTEQ: _op_gteq,
    EQEQ: _op_eq,
    NOTEQ: _op_ne,
    OP_IN: _op_in,
    TM_DEL: _op_del
}

def pyeval(src, glo_vars = None, debug = False):
    # this will save a little memory
    # because varg will always asigned with given value.
    glo_vars = glo_vars or {}
    code = compile(src)
    ins_list = split_instr(code)
    # print(ins_list)
    loc_vars = []
    stack = []
    r = None
    idx = 0
    cyc = 0
    while idx < len(ins_list):
        op,v = ins_list[idx]
        if debug:
            print(' ' * 10, cyc, tmcodes[op], v)
        cyc += 1
        if op == LOAD_CONSTANT:
            r = getConst(v)
            stack.append(r)
            if debug:
                print(' ' * 30, '<==' + str(r))
        elif op == LOAD_LOCAL:
            r = loc_vars[v]
            stack.append(r)
        elif op == STORE_LOCAL:
            r = stack.pop()
            loc_vars[v] = r
        elif op == LOAD_GLOBAL:
            name = getConst(v)
            if debug:
                print(' ' * 30, '<==' + name)
            if name in glo_vars:
                r = glo_vars[name]
            else:
                r = __builtins__[name]
            stack.append(r)
        elif op == STORE_GLOBAL:
            name = getConst(v)
            if debug:
                print(' '* 30, '==>' + name)
            r = stack.pop()
            glo_vars[name] = r
        elif op in op_dict:
            y = stack.pop()
            x = stack.pop()
            r = op_dict[op](x,y)
            stack.append(r)
        elif op == POP:
            r = stack.pop()
        elif op == CALL:
            args = []
            i = 0
            while i < v:
                args.append(stack.pop())
                i+=1
            args.reverse()
            r = apply(stack.pop(), args)
            stack.append(r)
        elif op == LIST:
            r = []
            stack.append(r)
        elif op == LIST_APPEND:
            x = stack.pop()
            r = stack[-1]
            r.append(x)
        elif op == DICT:
            r = {}
            stack.append(r)
        elif op == SET:
            k = stack.pop()
            r = stack.pop()
            v = stack.pop()
            r[k] = x
        elif op == DICT_SET:
            v = stack.pop()
            k = stack.pop()
            r = stack[-1]
            r[k] = v
        elif op == LOAD_GLOBALS:
            r = glo_vars
            stack.append(r)
        elif op == TM_EOP:
            pass
        elif op == LOAD_NONE:
            r = None
            stack.append(r)
        elif op == UP_JUMP:
            idx -= v
            continue
        elif op == ITER_NEW:
            r = iter(stack.pop())
            stack.append(r)
        elif op == TM_NEXT:
            try:
                r = next(stack[-1])
                stack.append(r)
            except Exception as e:
                print(e)
                idx += v
        elif op == TM_UNARRAY:
            collection = stack.pop()
            if len(collection) != v:
                raise("TM_UNARRAY expect length %s but see %s".format(v, len(collection)))
            collection.reverse()
            for x in collection:
                stack.append(x)
        elif op == SETJUMP:
            print("SETJUMP not implemented")
        elif op == POP_JUMP_ON_FALSE:
            r = stack.pop()
            if not r:
                idx += v
                r = None
                continue
        elif op == TM_ROT:
            i = 0
            lst = []
            while i < v:
                lst.append(stack.pop())
                i += 1
            lst.reverse()
            for i in lst:
                stack.append(i)
            r = None
        else:
            raise(sformat("unknown handled code %s:\"%s\"\n", op, tmcodes[op]))
        idx += 1
    return r

if __name__ == "__main__":
    argc = len(ARGV)
    argv = ARGV
    if argc == 2:
        text = load(argv[1])
        pyeval(text, None, True)



