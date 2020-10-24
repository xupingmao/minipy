# -*- coding:utf-8 -*-
# @author xupingmao <578749341@qq.com>
# @since 2020/10/18 00:32:28
# @modified 2020/10/24 16:08:43

from mp_encode import *
from mp_opcode import *

class CodeWriter:

    def __init__(self, code):
        self.code = code

    def indent(self):
        pass

    def writeline(self, line):
        print(line)


def convert(code):
    inst_list = compile_to_list(code, "test.py")
    writer = CodeWriter(code)
    module_name = None

    for op, val in inst_list:
        # print(op, val)
        if op == OP_FILE:
            writer.writeline("Object module_file = string_new(\"%s\");" % val)
            module_name = val.replace(".", "_")
        elif op == OP_DEF:
            line = "Object {}_{}() {".format(module_name, val)
            writer.writeline(line)
        elif op == OP_LOAD_PARAMS:
            line = "  /* OP_LOAD_PARAMS */"
            writer.writeline(line)
        elif op == OP_LOAD_LOCAL:
            line = "  MP_PUSH(locals[{}]);".format(val)
            writer.writeline(line)
        elif op == OP_NUMBER:
            line = "  MP_PUSH(number_obj({}));".format(val)
            writer.writeline(line)
        elif op == OP_STRING:
            line = "  MP_PUSH(string_new(\"{}\"));".format(val)
            writer.writeline(line)
        elif op == OP_LTEQ:
            writer.writeline("  L1 = MP_POP();")
            writer.writeline("  L2 = MP_POP();")
            line = "  MP_PUSH(obj_LTEQ(L1, L2));";
            writer.writeline(line)
        elif op == OP_POP_JUMP_ON_FALSE:
            writer.writeline("  if(!is_true_obj(MP_POP()) {")
            writer.writeline("    goto TAG_{};".format(val))
            writer.writeline("  }")
        elif op == OP_LOAD_GLOBAL:
            writer.writeline("  MP_PUSH(load_global(\"{}\");".format(val))
        elif op == OP_CALL:
            writer.writeline("  MP_PUSH(call_function({}));".format(val));
        elif op == OP_POP:
            writer.writeline("  MP_POP();")
        elif op == OP_JUMP:
            writer.writeline("  goto TAG_{};".format(val))
        elif op == OP_LINE:
            continue
        elif op == OP_EQEQ:
            writer.writeline("  MP_PUSH(obj_EQEQ(MP_POP(), MP_POP());")
        else:
            op_str = opcodes[op]
            raise "Unknown opcode, key={}, val={}".format(op_str, val)


code = load("test/mp2c/fib.py")
convert(code)


