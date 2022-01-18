# -*- coding:utf-8 -*-
# @author xupingmao <578749341@qq.com>
# @since 2020/10/18 00:32:28
# @modified 2022/01/18 21:36:56

import sys
import os
from mp_encode import *
from mp_opcode import *

class CodeWriter:

    def __init__(self, code, name):
        self.code = code
        self.name = name
        self.line_buf = []
        self.func_buf = []
        self.label_dict = {}
        self.is_in_func = False
        self.writeline("MpObj %s_main() {" % self.name)
        self.writeline("  MpObj fname = string_new(\"%s\");" % self.name)
        self.writeline("  MpObj module_name = fname;")
        self.writeline("  module = module_new(fname, module_name, NONE_OBJECT);")
        self.writeline("  globals = GET_MODULE(module)->globals;")
        self.writeline("  MpObj t1, t2, t3;");
        self.writeline("  MpObj op_stack[256];")
        self.writeline("  MpObj *top = op_stack;")
        self.writeline("  MpObj func = NONE_OBJECT;")

    def indent(self):
        pass

    def writeline(self, line):
        if self.is_in_func:
            self.func_buf.append(["str", line])
        else:
            self.line_buf.append(["str", line])

    def goto_label(self, val, indent = 2):
        self.label_dict[val] = 1
        self.writeline(" " * indent + "goto L%s;" % val)

    def define_label(self, label):
        if self.is_in_func:
            buf = self.func_buf
        else:
            buf = self.line_buf

        buf.append(["label", label])

    def enter_func(self):
        self.is_in_func = True

    def exit_func(self):
        self.is_in_func = False

    def end(self):
        pass

    def gen_code_by_buf(self, buf):
        result = []
        for type, value in buf:
            if type == "label":
                if value in self.label_dict:
                    result.append("L%s:" % value)
            else:
                result.append(value)
        return "\n".join(result)

    def gen_main_code(self):
        self.writeline("  return NONE_OBJECT;")
        self.writeline("}")
        return self.gen_code_by_buf(self.line_buf)

    def get_code(self):
        main_code = self.gen_main_code()
        func_code = self.gen_code_by_buf(self.func_buf)
        buf = []
        buf.append("#include \"../src/include/mp.h\"")
        buf.append("#include \"../mp2c/mp2c.c\"")
        buf.append("static MpObj module;")
        buf.append("static MpObj globals;")
        buf.append(func_code)
        buf.append(main_code)
        return "\n".join(buf)

def do_convert_op(writer, op, val, func_name):
    # writer.writeline("  t2 = MP2C_POP();")
    # writer.writeline("  t1 = MP2C_POP();")
    # writer.writeline("  MP2C_PUSH(%s(t1, t2));" % func_name)
    writer.writeline("  top--;")
    writer.writeline("  *top = %s(*top, *(top+1));" % func_name)

def convert(code, writer):
    inst_list = compile_to_list(code, "test.py")
    module_name = None
    func_name   = None
    op_index = 0
    code_list = code.split("\n")

    for op, val in inst_list:
        writer.define_label(op_index)
        op_index += 1

        # print(op, val)
        if op == OP_FILE:
            writer.writeline("  MpObj module_file = string_new(\"%s\");" % val)
            module_name = val.replace(".", "_")
        elif op == OP_DEF:
            func_name = val
            func_name_full = "{}_{}".format(module_name, func_name)
            t = func_name_full
            writer.writeline("  func = mp2c_def_func(module, \"%s\", %s);" % (func_name,t))
            writer.writeline("  MP2C_PUSH(func);")

            # 进入函数的作用域
            writer.enter_func()
            writer.writeline("")
            line = "MpObj {}() {{".format(func_name_full)
            writer.writeline(line)
            writer.writeline("  MpObj locals[256];")
            writer.writeline("  MpObj op_stack[256];")
            writer.writeline("  MpObj *top = op_stack;")
            writer.writeline("  MpObj t1, t2, t3;")
        elif op == OP_LOAD_PARAMS:
            parg = int(val / 256)
            varg = val % 256
            line = "  /* OP_LOAD_PARAMS(%s) */" % parg
            writer.writeline(line)
            for i in range(parg):
                writer.writeline("""  locals[%d] = arg_take_obj("%s");""" % (i, func_name))
        elif op == OP_LOAD_LOCAL:
            line = "  MP2C_PUSH(locals[{}]);".format(val)
            writer.writeline(line)
        elif op == OP_NUMBER:
            line = "  MP2C_PUSH(number_obj({}));".format(val)
            writer.writeline(line)
        elif op == OP_STRING:
            line = "  MP2C_PUSH(string_new(\"{}\"));".format(val)
            writer.writeline(line)
        elif op == OP_LOAD_GLOBAL:
            writer.writeline("  MP2C_PUSH(mp2c_load_global(globals, \"{}\"));".format(val))
        elif op == OP_CALL:
            argc = val
            writer.writeline("  top -= %s; /* args */" % argc)
            writer.writeline("  *top = obj_call_narg(*top, %s, top + 1);" % argc)
        elif op == OP_POP:
            writer.writeline("  MP2C_POP();")
        elif op == OP_JUMP:
            writer.goto_label(op_index + val)
        elif op == OP_LINE:
            index = val - 1
            if index < len(code_list):
                line = code_list[index]
                line = line.rstrip()
            else:
                line = "Unknown"
            writer.writeline("  /* (line:%s) %r */" % (val, line))
            # writer.writeline("L%s:" % val)
        elif op == OP_POP_JUMP_ON_FALSE:
            writer.writeline("  if(!is_true_obj(MP2C_POP())) {")
            writer.goto_label(op_index + val, 4)
            writer.writeline("  }")
        elif op == OP_JUMP_ON_TRUE:
            writer.writeline("  if (is_true_obj(MP2C_POP())) {");
            writer.goto_label(op_index + val, 4)
            writer.writeline("  }");
        elif op == OP_RETURN:
            writer.writeline("  return MP2C_POP();")
        elif op == OP_OR:
            do_convert_op(writer, op, val, "obj_or")
        elif op == OP_SUB:
            do_convert_op(writer, op, val, "obj_sub")
        elif op == OP_ADD:
            do_convert_op(writer, op, val, "obj_add")
        elif op == OP_MOD:
            do_convert_op(writer, op, val, "obj_mod")
        elif op == OP_LTEQ:
            do_convert_op(writer, op, val, "obj_LTEQ")
        elif op == OP_EQEQ:
            do_convert_op(writer, op, val, "obj_EQEQ")
        elif op == OP_EOF:
            writer.writeline("  /* EOF: end of function */")
            writer.writeline("}")
            writer.exit_func()
        elif op == OP_STORE_GLOBAL:
            writer.writeline("  obj_set_by_cstr(globals, \"%s\", MP2C_POP());" % val)
        elif op == OP_EOP:
            writer.writeline("  /* EOP */")
        elif op == OP_IMPORT:
            if val == 1:
                writer.writeline("  _import(mp_globals, MP2C_POP());")
            else:
                writer.writeline("  _import(mp_globals, *(top-1), *top);")
                writer.writeline("  top -= 2;")
        else:
            op_str = opcodes[op]
            message = "Unknown opcode, key={}, val={}".format(op_str, val)
            raise message

    writer.end()

def file_basename(fpath):
    fname = fpath.split("/")[-1]
    name = fname.split(".")[0]
    return name

def main():
    argv = sys.argv
    # print(argv)

    if len(argv) == 2:
        fpath = argv[1]
    elif len(argv) == 1:
        fpath = "test/mp2c/fib.py"
    else:
        raise Exception("参数异常")

    code = load(fpath)    
    name = file_basename(fpath)
    writer = CodeWriter(code, name)
    convert(code, writer)
    result_code = writer.get_code()
    # print(result_code)

    c_file_path = "build/%s.c" % name
    save(c_file_path, result_code)
    print("saved to %s" % c_file_path)

    print("start build...")
    os.system("gcc build/mp2c_main.c -o build/mp2c")


if __name__ == '__main__':
    main()
