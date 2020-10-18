# -*- coding:utf-8 -*-
# @author xupingmao <578749341@qq.com>
# @since 2020/10/18 00:32:28
# @modified 2020/10/19 00:59:54

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
        elif op == OP_LINE:
            continue
        else:
            op_str = opcodes[op]
            raise "Unknown opcode, key={}, val={}".format(op_str, val)


code = load("test/mp2c/fib.py")
convert(code)


