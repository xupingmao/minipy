# -*- coding:utf-8 -*-
# @author xupingmao <578749341@qq.com>
# @since 2020/10/18 00:32:28
# @modified 2021/10/01 10:52:01

from mp_encode import *
from mp_opcode import *

class CodeWriter:

    def __init__(self, code):
        self.code = code
        self.line_buf = []
        self.label_dict = {}

    def indent(self):
        pass

    def writeline(self, line):
        self.line_buf.append(["str", line])

    def goto_label(self, val, indent = 2):
        self.label_dict[val] = 1
        self.writeline(" " * indent + "goto L%s;" % val)

    def define_label(self, label):
        self.line_buf.append(["label", label])

    def end(self):
        for type, value in self.line_buf:
            if type == "label":
                if value in self.label_dict:
                    print("L%s:" % value)
            else:
                print(value)

def do_convert_op(writer, op, val, func_name):
    writer.writeline("  t1 = MP_POP();")
    writer.writeline("  t2 = MP_POP();")
    writer.writeline("  MP_PUSH(%s(t1, t2);" % func_name)

def convert(code):
    inst_list = compile_to_list(code, "test.py")
    writer = CodeWriter(code)
    module_name = None
    op_index = 0

    for op, val in inst_list:
        writer.define_label(op_index)
        op_index += 1

        # print(op, val)
        if op == OP_FILE:
            writer.writeline("MpObj module_file = string_new(\"%s\");" % val)
            module_name = val.replace(".", "_")
        elif op == OP_DEF:
            writer.writeline("")
            line = "MpObj {}_{}() {{".format(module_name, val)
            writer.writeline(line)
        elif op == OP_LOAD_PARAMS:
            line = "  /* OP_LOAD_PARAMS(%s) */" % val
            writer.writeline(line)
            writer.writeline("  MpObj t1, t2;")
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
            writer.writeline("  t1 = MP_POP();")
            writer.writeline("  t2 = MP_POP();")
            line = "  MP_PUSH(obj_LTEQ(t1, t2));";
            writer.writeline(line)
        elif op == OP_LOAD_GLOBAL:
            writer.writeline("  MP_PUSH(load_global(\"{}\");".format(val))
        elif op == OP_CALL:
            writer.writeline("  t1 = TM_POP();")
            writer.writeline("  MP_PUSH(call_function(t1));");
        elif op == OP_POP:
            writer.writeline("  MP_POP();")
        elif op == OP_JUMP:
            writer.goto_label(op_index + val)
        elif op == OP_LINE:
            writer.writeline("  /* line:%s */" % val)
            # writer.writeline("L%s:" % val)
        elif op == OP_EQEQ:
            writer.writeline("  t1 = MP_POP();")
            writer.writeline("  t2 = MP_POP();")
            writer.writeline("  MP_PUSH(obj_EQEQ(t1, t2;")

        elif op == OP_POP_JUMP_ON_FALSE:
            writer.writeline("  if(!is_true_obj(MP_POP()) {")
            writer.goto_label(op_index + val, 4)
            writer.writeline("  }")
        elif op == OP_JUMP_ON_TRUE:
            writer.writeline("  if (is_true_obj(MP_POP()) {");
            writer.goto_label(op_index + val, 4)
            writer.writeline("  }");
        elif op == OP_OR:
            writer.writeline("  MP_PUSH(obj_or(MP_POP(), MP_POP());")
        elif op == OP_RETURN:
            writer.writeline("  return TM_POP();")
        elif op == OP_SUB:
            do_convert_op(writer, op, val, "obj_sub")
        elif op == OP_ADD:
            do_convert_op(writer, op, val, "obj_add")
        elif op == OP_EOF:
            writer.writeline("  /* EOF: end of function */")
            writer.writeline("}")
        elif op == OP_STORE_GLOBAL:
            writer.writeline("  tm_set_global(%r, MP_POP());" % val)
        elif op == OP_EOP:
            writer.writeline("  /* EOP */")
        else:
            op_str = opcodes[op]
            message = "Unknown opcode, key={}, val={}".format(op_str, val)
            print(message)
            raise

    writer.end()


code = load("test/mp2c/fib.py")
convert(code)


