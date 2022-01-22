# -*- coding:utf-8 -*-
# @author xupingmao
# @since 2016
# @modified 2020/11/12 10:24:26

import mp_encode
compile         = mp_encode.compile
split_instr     = mp_encode.split_instr
compile_to_list = mp_encode.compile_to_list

from mp_opcode import *

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

OP_FUNC_DICT_2 = {
    OP_ADD: _op_add,
    OP_SUB: _op_sub,
    OP_DIV: _op_div,
    OP_MUL: _op_mul,
    OP_MOD: _op_mod,
    OP_GET: _op_get,
    OP_LT: _op_lt,
    OP_GT: _op_gt,
    OP_LTEQ: _op_lteq,
    OP_GTEQ: _op_gteq,
    OP_EQEQ: _op_eq,
    OP_NOTEQ: _op_ne,
    OP_IN: _op_in,
    OP_DEL: _op_del
}

def _op_neg(v):
    return -v

OP_FUNC_DICT_1 = {
    OP_NEG: _op_neg
}

op_skip = {
    OP_LINE:    "OP_LINE",
    OP_SETJUMP: "OP_SETJUMP",
    OP_FILE:    "OP_FILE",
}

class Env:
    def __init__(self, globals):
        self._glo = globals
        
    def globals(self):
        return self._glo

class Emulator:

    def record_debug_line(self, cyc, op, v):
        if not self.debug:
            return

        self.line = str(cyc).ljust(5) + opcodes[op].ljust(22) + str(v).ljust(20)
        # print(line)

    def print_debug_line(self):
        if self.debug:
            print(self.line)

    def append_object(self, r):
        if not self.debug:
            return

        if istype(r,"string"):
            self.line+='"' + r + '"'
        else:
            self.line += str(r)

    def pyeval(self, src, glo_vars = None, debug = False):
        # this will save a little memory
        # because varg will always asigned with given value.
        self.debug = debug
        self.line  = ""

        glo_vars = glo_vars or {}
        env = Env(glo_vars)
        glo_vars['globals'] = env.globals
        # code = compile(src, "<shell>")
        ins_list = compile_to_list(src, "<shell>")
        # print(ins_list)
        loc_vars = []
        stack = []
        r = None
        idx = 0
        cyc = 0
        while idx < len(ins_list):
            op,v = ins_list[idx]

            self.record_debug_line(cyc, op, v)

            cyc += 1
            if op == OP_CONSTANT:
                r = v
                stack.append(r)
                self.append_object(r)
            elif op == OP_STRING:
                r = v
                stack.append(r)
                self.append_object(r)
            elif op == OP_NUMBER:
                r = v
                stack.append(r)
                self.append_object(r)
            elif op == OP_LOAD_LOCAL:
                r = loc_vars[v]
                stack.append(r)
            elif op == OP_STORE_LOCAL:
                r = stack.pop()
                loc_vars[v] = r
            elif op == OP_LOAD_GLOBAL:
                name = v
                if debug:
                    line += name
                if name in glo_vars:
                    r = glo_vars[name]
                else:
                    r = __builtins__[name]
                stack.append(r)
            elif op == OP_STORE_GLOBAL:
                name = v
                if debug:
                    line += name
                r = stack.pop()
                glo_vars[name] = r
            elif op == OP_IMPORT:
                if v == 1:
                    a = stack.pop()
                    r = _import(glo_vars, a)
                else:
                    b = stack.pop()
                    a = stack.pop()
                    r = _import(glo_vars, a, b)
            elif op in OP_FUNC_DICT_2:
                y = stack.pop()
                x = stack.pop()
                r = OP_FUNC_DICT_2[op](x,y)
                stack.append(r)
            elif op in OP_FUNC_DICT_1:
                v = stack.pop()
                r = OP_FUNC_DICT_1[op](v)
            elif op == OP_POP:
                r = stack.pop()
            elif op == OP_CALL:
                args = []
                i = 0
                while i < v:
                    args.append(stack.pop())
                    i+=1
                args.reverse()
                r = apply(stack.pop(), args)
                stack.append(r)
            elif op == OP_LIST:
                r = []
                stack.append(r)
            elif op == OP_APPEND:
                x = stack.pop()
                r = stack[-1]
                r.append(x)
            elif op == OP_DICT:
                r = {}
                stack.append(r)
            elif op == OP_SET:
                k = stack.pop()
                r = stack.pop()
                v = stack.pop()
                r[k] = v
            elif op == OP_DICT_SET:
                v = stack.pop()
                k = stack.pop()
                r = stack[-1]
                r[k] = v
            elif op == OP_EOP:
                pass
            elif op == OP_NONE:
                r = None
                stack.append(r)
            elif op == OP_UP_JUMP:
                idx -= v
                self.print_debug_line()
                continue
            elif op == OP_JUMP:
                idx += v
                self.print_debug_line()
                continue
            elif op == OP_ITER:
                r = iter(stack.pop())
                stack.append(r)
            elif op == OP_NEXT:
                try:
                    r = next(stack[-1])
                    stack.append(r)
                except Exception as e:
                    print(e)
                    idx += v
            elif op == OP_UNPACK:
                collection = stack.pop()
                if len(collection) != v:
                    raise("MP_UNARRAY expect length %s but see %s".format(v, len(collection)))
                collection.reverse()
                for x in collection:
                    stack.append(x)
            elif op == OP_APPLY:
                args = stack.pop()
                fnc = stack.pop()
                r = fnc(*args)
            elif op == OP_SLICE:
                second = stack.pop()
                first = stack.pop()
                obj = stack.pop()
                r = obj[first:second]
                stack.append(r)
            elif op in op_skip:
                pass
            elif op == OP_POP_JUMP_ON_FALSE:
                r = stack.pop()
                if not r:
                    idx += v
                    r = None
                    self.print_debug_line()
                    continue
            elif op == OP_ROT:
                i = 0
                lst = []
                while i < v:
                    lst.append(stack.pop())
                    i += 1
                for i in lst:
                    stack.append(i)
                r = None
            else:
                raise(sformat("unknown handled code %s:\"%s\"\n", op, opcodes[op]))
            
            self.print_debug_line()
            idx += 1
        return r

def pyeval(src, glo_vars = None, debug = False):
    emulator = Emulator()
    return emulator.pyeval(src, glo_vars, debug)


if __name__ == "__main__":
    argc = len(ARGV)
    argv = ARGV
    if argc == 2:
        text = load(argv[1])
        pyeval(text, None, True)